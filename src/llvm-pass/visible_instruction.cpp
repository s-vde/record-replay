
#include "visible_instruction.hpp"
#include "llvm/IR/IRBuilder.h"
#include "color_output.hpp"
#include "instrumentation_utils.hpp"
#include "print.hpp"

using Color = utils::io::Color;

namespace record_replay
{
    VisibleInstruction::VisibleInstruction(
        const Op& op,
        llvm::GlobalVariable* var,
        llvm::Value* index)
    : mOp(op)
    , mGVar(var)
    , mIndex(index) { }
    
    const Op& VisibleInstruction::op() const
    {
        return mOp;
    }
    
    llvm::Value* VisibleInstruction::object(llvm::Module& M, llvm::Instruction* I) const
    {
        PRINTF("\t" << utils::io::text_color("\t\tVisibleInstruction", Color::YELLOW), "object", "", "\n");
        llvm::Value* index = nullptr;
        if (mIndex != nullptr) {
            if (llvm::ConstantInt* cint = llvm::dyn_cast<llvm::ConstantInt>(mIndex)) {
                index = llvm::AddrSpaceCastInst::CreateIntegerCast(
                    cint,
                    llvm::IntegerType::getInt32Ty(M.getContext()),
                    false,
                    "",
                    I
                );
            } else if (llvm::SExtInst* sext = llvm::dyn_cast<llvm::SExtInst>(mIndex)) {
                index = sext->getOperand(0);
            } else {
                PRINT("UNHANDLED index case\n");
            }
		} else { index = llvm::ConstantInt::get(M.getContext(), llvm::APInt(32, llvm::StringRef("0"), 10)); }
        llvm::AllocaInst* obj = new llvm::AllocaInst(
            M.getTypeByName("class.program_model::Object"),
            "obj",
            I
        );
        llvm::CallInst::Create(
            llvm::cast<llvm::Function>(M.getFunction(instrumentation_utils::get_mangled_name(M, "program_model", "", "llvm_object"))),
            { obj, instrumentation_utils::create_global_cstring_const(M, "", mGVar->getName().str()), index },
            "",
            I
        );
        return obj;
    }
    
    std::pair<bool,VisibleInstruction> is_visible(llvm::Instruction* I)
    {
        PRINTF(utils::io::text_color("\t\trecord_replay", Color::YELLOW), "is_visible", "", ""); I->dump();
        // READ
        if (llvm::LoadInst* LI = llvm::dyn_cast<llvm::LoadInst>(I)) {
            return record_replay::is_visible(Op::READ, LI->getPointerOperand());
        }
        // WRITE
        else if (llvm::StoreInst* SI = llvm::dyn_cast<llvm::StoreInst>(I)) {
            return record_replay::is_visible(Op::WRITE, SI->getPointerOperand());
        }
        else if (llvm::CallInst* CI = llvm::dyn_cast<llvm::CallInst>(I)) {
            llvm::Function* callee = CI->getCalledFunction();
            // LOCK
            if (callee->getName() == "pthread_mutex_lock") {
                return record_replay::is_visible(Op::LOCK, CI->getArgOperand(0));
            }
            // UNLOCK
            else if(callee->getName() == "pthread_mutex_unlock") {
                return record_replay::is_visible(Op::UNLOCK, CI->getArgOperand(0));
            }
        }
        // ELSE
        else {
            PRINT("\t\t" << text_color("Unhandled/Invisible Instruction ", Color::RED) << "\n");
        }
        return std::pair<bool,VisibleInstruction>(false, VisibleInstruction());
    }
    
    std::pair<bool,VisibleInstruction> is_visible(const Op& op, llvm::Value* operand)
    {
        PRINTF(utils::io::text_color("\t\t\trecord_replay", Color::YELLOW), "is_visible", "", "");
        operand->dump();
        // 1. If addr is a GlobalVariable
        if (llvm::GlobalVariable* gvar = llvm::dyn_cast<llvm::GlobalVariable>(operand))
        {
            PRINT("\t\t\t\t" << text_color("GlobalVariable ", Color::GREEN) << "\n");
            return std::pair<bool,VisibleInstruction>(true, VisibleInstruction(op, gvar, nullptr));
        }
        // 2. If addr is a Constant C
        else if (llvm::Constant* C = llvm::dyn_cast<llvm::Constant>(operand))
        {
            // 2.1. If C is a ConstantExpression CE
            if (llvm::ConstantExpr* CE = llvm::dyn_cast<llvm::ConstantExpr>(C)) {
                // 2.1.1. If CE is a GEP
                if (CE->isGEPWithNoNotionalOverIndexing()) {
                    return is_visible(op, CE->getOperand(0), CE->getOperand(1), CE->getOperand(2));
                }
                // 2.1.2. Otherwise UNHANDLED
                else { PRINT("\t\t\t\t" << text_color("Unhandled ConstantExpr", Color::RED) << "\n"); }
            }
            // 2.2. Otherwise UNHANDLED
            else { PRINT("\t\t\t\t" << text_color("Unhandled Constant", Color::RED) << "\n"); }
        }
        // 3. If addr is an Instruction
        else if (llvm::Instruction* I = llvm::dyn_cast<llvm::Instruction>(operand))
        {
            // 3.1. If instr is a GetElementPtrInst
            // GEP's are involved in computing addresses.
            /// @see http://llvm.org/docs/GetElementPtr.html
            /// @see http://llvm.org/docs/doxygen/html/classllvm_1_1GetElementPtrInst.html
            if (llvm::GetElementPtrInst* gep = llvm::dyn_cast<llvm::GetElementPtrInst>(I)) {
                // 3.1.1. If gep's pointer operand is not a struct type
                if (!gep->getPointerOperand()->getType()->isStructTy()) {
					if (op == Op::LOCK || op == Op::UNLOCK) {
						if (llvm::ConstantExpr* gep2 = llvm::dyn_cast<llvm::ConstantExpr>(gep->getPointerOperand())) {
							return is_visible(op, gep2->getOperand(0), gep2->getOperand(1), gep->getOperand(1));
						}
					}
                    return
                        is_visible(
                            op,
                            gep->getPointerOperand(),
                            gep->getOperand(1),
                            gep->getOperand(2)
                        );
                // 3.1.2. Otherwise UNHANDLED
                } else { PRINT("\t\t\t\t" << text_color("Unhandled GEP with struct type ptrop", Color::RED) << "\n"); }
            }
            // 3.2. Otherwise UNHANDLED
            else { PRINT("\t\t\t\t" << text_color("Unhandled Instruction", Color::RED) << "\n"); }
        }
        // 4. Otherwise UNHANDLED
        else { PRINT("\t\t\t\t" << text_color("Unhandled Value", Color::RED) << "\n"); }
        return std::pair<bool,VisibleInstruction>(false, VisibleInstruction());
    }
    
    std::pair<bool,VisibleInstruction> is_visible(
        const Op& op,
        llvm::Value* ptrop,
        llvm::Value* indexop1,
        llvm::Value* indexop2)
    {
        PRINTF(utils::io::text_color("\t\t\t\trecord_replay", Color::YELLOW), "is_visible", "", "\n");
        if (llvm::GlobalVariable* gvar = llvm::dyn_cast<llvm::GlobalVariable>(ptrop)) {
            PRINT("\t\t\t\t\t" << text_color("GlobalVariable ", Color::GREEN) << "\n");
            return std::pair<bool,VisibleInstruction>(true, VisibleInstruction(op, gvar, indexop2));
        } else {
            PRINT("\t\t\t\t\t" << text_color("Unhandled GEP", Color::RED) << "\n");
            return std::pair<bool,VisibleInstruction>(false, VisibleInstruction());
        }
    }
} // end namespace record_replay
