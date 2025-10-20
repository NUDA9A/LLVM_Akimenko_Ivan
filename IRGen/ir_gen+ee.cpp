#include "../SDL/sim.h"

#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/GenericValue.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/raw_ostream.h"

#include <memory>

using namespace llvm;

int main() {
  LLVMContext context;
  // ; ModuleID = 'app.c'
  // source_filename = "app.c"
  auto module = std::make_unique<Module>("app.c", context);
  IRBuilder<> builder(context);

  Type *voidType = Type::getVoidTy(context);
  Type *int32Type = Type::getInt32Ty(context);
  Type *int64Type = Type::getInt64Ty(context);
  Type* int8Type   = Type::getInt8Ty(context);
  Type* int8PtrType= PointerType::getUnqual(int8Type);
  ArrayType *int32ArrayType = ArrayType::get(int32Type, 500);
  ArrayType *int32ArrayArrayType = ArrayType::get(int32ArrayType, 500);
  PointerType* int32ArrayPtrType = PointerType::getUnqual(int32ArrayType);
  ArrayType *int32x3ArrayType = ArrayType::get(int32Type, 3);
  auto* Init = ConstantArray::get(
      int32x3ArrayType,
      { builder.getInt32(-1), builder.getInt32(0), builder.getInt32(1) });
  auto* d = new GlobalVariable(
      *module,
      int32x3ArrayType,
      true,
      GlobalValue::PrivateLinkage,
      Init,
      "__const.count_alive_pixels.d");
  d->setUnnamedAddr(GlobalValue::UnnamedAddr::Global);
  d->setAlignment(Align(4));

  // declare void @simPutPixel(i32, i32, i32)
  // local_unnamed_addr #1
  ArrayRef<Type *> simPutPixelParamTypes = {int32Type, int32Type, int32Type};
  FunctionType *simPutPixelType =
      FunctionType::get(voidType, simPutPixelParamTypes, false);
  FunctionCallee simPutPixelFunc =
      module->getOrInsertFunction("simPutPixel", simPutPixelType);

  // declare void @simFlush()
  FunctionType *simFlushType = FunctionType::get(voidType, false);
  FunctionCallee simFlushFunc =
      module->getOrInsertFunction("simFlush", simFlushType);

  // declare i32 @simRand();
  FunctionType* simRandType = FunctionType::get(int32Type, false);
  FunctionCallee simRandFunc =
      module->getOrInsertFunction("simRand", simRandType);

  //auto* sizeBytes = builder.getInt64(500ull * 500ull * 4ull);

  //; Function Attrs: nounwind optsize uwtable
  // define dso_local void @fill_init_field([500 x i32]* nocapture noundef
  // writeonly %0) {
  ArrayRef<Type *> fill_init_fieldTypes = {int32ArrayPtrType};
  FunctionType *fill_init_fieldType =
      FunctionType::get(voidType, fill_init_fieldTypes, false);
  Function *fill_init_fieldFunc =
      Function::Create(fill_init_fieldType, Function::ExternalLinkage,
                       "fill_init_field", module.get());

  {
    fill_init_fieldFunc->addFnAttr(Attribute::NoUnwind);
    fill_init_fieldFunc->addFnAttr(Attribute::OptimizeForSize);
    fill_init_fieldFunc->addFnAttr(Attribute::UWTable);

    Argument *fill_init_fieldArg = fill_init_fieldFunc->getArg(0);
    fill_init_fieldArg->addAttr(Attribute::NoCapture);
    fill_init_fieldArg->addAttr(Attribute::NoUndef);
    fill_init_fieldArg->addAttr(Attribute::WriteOnly);

    BasicBlock *fill_init_field_entry_BB =
        BasicBlock::Create(context, "", fill_init_fieldFunc);
    BasicBlock *fill_init_field_BB2 =
        BasicBlock::Create(context, "", fill_init_fieldFunc);
    BasicBlock *fill_init_field_BB4 =
        BasicBlock::Create(context, "", fill_init_fieldFunc);
    BasicBlock *fill_init_field_BB5 =
        BasicBlock::Create(context, "", fill_init_fieldFunc);
    BasicBlock *fill_init_field_BB8 =
        BasicBlock::Create(context, "", fill_init_fieldFunc);
    BasicBlock *fill_init_field_BB13 =
        BasicBlock::Create(context, "", fill_init_fieldFunc);
    BasicBlock *fill_init_field_BB17 =
        BasicBlock::Create(context, "", fill_init_fieldFunc);
    BasicBlock *fill_init_field_BB19 =
        BasicBlock::Create(context, "", fill_init_fieldFunc);

    builder.SetInsertPoint(fill_init_field_entry_BB);
    //  br label %2
    builder.CreateBr(fill_init_field_BB2);
    // 2:                                                ; preds = %1, %5
    builder.SetInsertPoint(fill_init_field_BB2);
    //  %3 = phi i64 [ 0, %1 ], [ %6, %5 ]
    PHINode *val3 = builder.CreatePHI(int64Type, 2);
    //  br label %8
    builder.CreateBr(fill_init_field_BB8);
    // 4:                                                ; preds = %5
    builder.SetInsertPoint(fill_init_field_BB4);
    //  ret void
    builder.CreateRetVoid();
    //
    // 5:                                                ; preds = %19
    builder.SetInsertPoint(fill_init_field_BB5);
    //  %6 = add nuw nsw i64 %3, 1
    Value *val6 = builder.CreateAdd(val3, builder.getInt64(1), "", true, true);
    //  %7 = icmp eq i64 %6, 500
    Value *val7 = builder.CreateICmpEQ(val6, builder.getInt64(500));
    //  br i1 %7, label %4, label %2, !llvm.loop !5
    builder.CreateCondBr(val7, fill_init_field_BB4, fill_init_field_BB2);
    //
    // 8:                                                ; preds = %2, %19
    builder.SetInsertPoint(fill_init_field_BB8);
    //  %9 = phi i64 [ 0, %2 ], [ %20, %19 ]
    PHINode *val9 = builder.CreatePHI(int64Type, 2);
    //  %10 = tail call i32 @simRand() #6
    CallInst *val10 = builder.CreateCall(simRandFunc);
    val10->setTailCallKind(CallInst::TCK_Tail);
    //  %11 = and i32 %10, 1
    Value *val11 = builder.CreateAnd(val10, builder.getInt32(1));
    //  %12 = icmp eq i32 %11, 0
    Value *val12 = builder.CreateICmpEQ(val11, builder.getInt32(0));
    //  br i1 %12, label %19, label %13
    builder.CreateCondBr(val12, fill_init_field_BB19, fill_init_field_BB13);
    //
    // 13:                                               ; preds = %8
    builder.SetInsertPoint(fill_init_field_BB13);
    //  %14 = tail call i32 @simRand() #6
    CallInst *val14 = builder.CreateCall(simRandFunc);
    val14->setTailCallKind(CallInst::TCK_Tail);
    //  %15 = and i32 %14, 1
    Value *val15 = builder.CreateAnd(val14, builder.getInt32(1));
    //  %16 = icmp eq i32 %15, 0
    Value *val16 = builder.CreateICmpEQ(val15, builder.getInt32(0));
    //  br i1 %16, label %19, label %17
    builder.CreateCondBr(val16, fill_init_field_BB19, fill_init_field_BB17);
    //
    // 17:                                               ; preds = %13
    builder.SetInsertPoint(fill_init_field_BB17);
    //  %18 = getelementptr inbounds [500 x i32], [500 x i32]* %0, i64 %3, i64
    //  %9
    Value *val18 = builder.CreateInBoundsGEP(int32ArrayType, fill_init_fieldArg,
                                             {val3, val9});
    //  store i32 1, i32* %18, align 4, !tbaa !7
    StoreInst *storeInst = builder.CreateStore(builder.getInt32(1), val18);
    storeInst->setAlignment(Align(4));
    //  br label %19
    builder.CreateBr(fill_init_field_BB19);
    //
    // 19:                                               ; preds = %8, %13, %17
    builder.SetInsertPoint(fill_init_field_BB19);
    //  %20 = add nuw nsw i64 %9, 1
    Value *val20 = builder.CreateAdd(val9, builder.getInt64(1), "", true, true);
    //  %21 = icmp eq i64 %20, 500
    Value *val21 = builder.CreateICmpEQ(val20, builder.getInt64(500));
    //  br i1 %21, label %5, label %8, !llvm.loop !11
    builder.CreateCondBr(val21, fill_init_field_BB5, fill_init_field_BB8);
    //}
    val3->addIncoming(builder.getInt64(0), fill_init_field_entry_BB);
    val3->addIncoming(val6, fill_init_field_BB5);
    val9->addIncoming(builder.getInt64(0), fill_init_field_BB2);
    val9->addIncoming(val20, fill_init_field_BB19);
  }
  //; Function Attrs: nounwind optsize uwtable
  // define dso_local void @display_epoch([500 x i32]* nocapture noundef readonly %0) {
  ArrayRef<Type *> display_epochTypes = {int32ArrayPtrType};
  FunctionType *display_epochType =
      FunctionType::get(voidType, display_epochTypes, false);
  Function *display_epochFunc =
      Function::Create(display_epochType, Function::ExternalLinkage,
                       "display_epoch", module.get());
  {
    display_epochFunc->addFnAttr(Attribute::NoUnwind);
    display_epochFunc->addFnAttr(Attribute::OptimizeForSize);
    display_epochFunc->addFnAttr(Attribute::UWTable);

    Argument *display_epochArg = display_epochFunc->getArg(0);
    display_epochArg->addAttr(Attribute::NoCapture);
    display_epochArg->addAttr(Attribute::NoUndef);
    display_epochArg->addAttr(Attribute::ReadOnly);

    BasicBlock *display_epoch_entry_BB =
        BasicBlock::Create(context, "", display_epochFunc);
    BasicBlock *display_epoch_BB2 =
        BasicBlock::Create(context, "", display_epochFunc);
    BasicBlock *display_epoch_BB6 =
        BasicBlock::Create(context, "", display_epochFunc);
    BasicBlock *display_epoch_BB7 =
        BasicBlock::Create(context, "", display_epochFunc);
    BasicBlock *display_epoch_BB10 =
        BasicBlock::Create(context, "", display_epochFunc);
    BasicBlock *display_epoch_BB16 =
        BasicBlock::Create(context, "", display_epochFunc);
    BasicBlock *display_epoch_BB17 =
        BasicBlock::Create(context, "", display_epochFunc);
    BasicBlock *display_epoch_BB18 =
        BasicBlock::Create(context, "", display_epochFunc);

    builder.SetInsertPoint(display_epoch_entry_BB);
    //  br label %2
    builder.CreateBr(display_epoch_BB2);
    //
    // 2:                                                ; preds = %1, %7
    builder.SetInsertPoint(display_epoch_BB2);
    //  %3 = phi i64 [ 0, %1 ], [ %8, %7 ]
    PHINode *val3 = builder.CreatePHI(int64Type, 2);
    //  %4 = trunc i64 %3 to i32
    Value* val4 = builder.CreateTrunc(val3, int32Type);
    //  %5 = trunc i64 %3 to i32
    Value* val5 = builder.CreateTrunc(val3, int32Type);
    //  br label %10
    builder.CreateBr(display_epoch_BB10);
    //
    // 6:                                                ; preds = %7
    builder.SetInsertPoint(display_epoch_BB6);
    //  ret void
    builder.CreateRetVoid();
    //
    // 7:                                                ; preds = %18
    builder.SetInsertPoint(display_epoch_BB7);
    //  %8 = add nuw nsw i64 %3, 1
    Value* val8 = builder.CreateAdd(val3, builder.getInt64(1), "", true, true);
    //  %9 = icmp eq i64 %8, 500
    Value* val9 = builder.CreateICmpEQ(val8, builder.getInt64(500));
    //  br i1 %9, label %6, label %2, !llvm.loop !12
    builder.CreateCondBr(val9, display_epoch_BB6, display_epoch_BB2);
    //
    // 10:                                               ; preds = %2, %18
    builder.SetInsertPoint(display_epoch_BB10);
    //  %11 = phi i64 [ 0, %2 ], [ %19, %18 ]
    PHINode* val11 = builder.CreatePHI(int64Type, 2);
    //  %12 = getelementptr inbounds [500 x i32], [500 x i32]* %0, i64 %3, i64 %11
    Value *val12 = builder.CreateInBoundsGEP(int32ArrayType, display_epochArg,
                                             {val3, val11});
    //  %13 = load i32, i32* %12, align 4, !tbaa !7
    LoadInst* val13 = builder.CreateLoad(int32Type, val12);
    val13->setAlignment(Align(4));
    //  %14 = icmp eq i32 %13, 0
    Value* val14 = builder.CreateICmpEQ(val13, builder.getInt32(0));
    //  %15 = trunc i64 %11 to i32
    Value* val15 = builder.CreateTrunc(val11, int32Type);
    //  br i1 %14, label %17, label %16
    builder.CreateCondBr(val14, display_epoch_BB17, display_epoch_BB16);
    //
    // 16:                                               ; preds = %10
    builder.SetInsertPoint(display_epoch_BB16);
    //  tail call void @simPutPixel(i32 noundef %4, i32 noundef %15, i32 noundef 16777215) #6
    CallInst* simPutPixelCall = builder.CreateCall(
        simPutPixelFunc,
        {val4, val15, builder.getInt32(16777215)});
    simPutPixelCall->setTailCallKind(CallInst::TCK_Tail);
    //  br label %18
    builder.CreateBr(display_epoch_BB18);
    //
    // 17:                                               ; preds = %10
    builder.SetInsertPoint(display_epoch_BB17);
    //  tail call void @simPutPixel(i32 noundef %5, i32 noundef %15, i32 noundef 0)
    CallInst* simPutPixelCall1 = builder.CreateCall(
        simPutPixelFunc,
        {val5, val15, builder.getInt32(0)});
    simPutPixelCall1->setTailCallKind(CallInst::TCK_Tail);
    //  br label %18
    builder.CreateBr(display_epoch_BB18);
    //
    // 18:                                               ; preds = %16, %17
    builder.SetInsertPoint(display_epoch_BB18);
    //  %19 = add nuw nsw i64 %11, 1
    Value* val19 = builder.CreateAdd(val11, builder.getInt64(1), "", true, true);
    //  %20 = icmp eq i64 %19, 500
    Value* val20 = builder.CreateICmpEQ(val19, builder.getInt64(500));
    //  br i1 %20, label %7, label %10, !llvm.loop !13
    builder.CreateCondBr(val20, display_epoch_BB7, display_epoch_BB10);
    //}

    val3->addIncoming(builder.getInt64(0), display_epoch_entry_BB);
    val3->addIncoming(val8, display_epoch_BB7);
    val11->addIncoming(builder.getInt64(0), display_epoch_BB2);
    val11->addIncoming(val19, display_epoch_BB18);
  }
  //; Function Attrs: nofree norecurse nosync nounwind optsize readonly uwtable
  // define i32 @count_alive_pixels(i32 noundef %0, i32 noundef %1, [500 x i32]* nocapture noundef readonly %2) {
  ArrayRef<Type *> count_alive_pixelsTypes = {int32Type, int32Type,
                                              int32ArrayPtrType};
  FunctionType *count_alive_pixelsType =
      FunctionType::get(int32Type, count_alive_pixelsTypes, false);
  Function *count_alive_pixelsFunc =
      Function::Create(count_alive_pixelsType, Function::ExternalLinkage,
                       "count_alive_pixels", module.get());
  {
    count_alive_pixelsFunc->addFnAttr(Attribute::NoFree);
    count_alive_pixelsFunc->addFnAttr(Attribute::NoRecurse);
    count_alive_pixelsFunc->addFnAttr(Attribute::NoSync);
    count_alive_pixelsFunc->addFnAttr(Attribute::NoUnwind);
    count_alive_pixelsFunc->addFnAttr(Attribute::OptimizeForSize);
    count_alive_pixelsFunc->addFnAttr(Attribute::ReadOnly);
    count_alive_pixelsFunc->addFnAttr(Attribute::UWTable);

    Argument *count_alive_pixelsArg1 = count_alive_pixelsFunc->getArg(0);
    count_alive_pixelsArg1->addAttr(Attribute::NoUndef);
    Argument *count_alive_pixelsArg2 = count_alive_pixelsFunc->getArg(1);
    count_alive_pixelsArg2->addAttr(Attribute::NoUndef);
    Argument *count_alive_pixelsArg3 = count_alive_pixelsFunc->getArg(2);
    count_alive_pixelsArg3->addAttr(Attribute::NoCapture);
    count_alive_pixelsArg3->addAttr(Attribute::NoUndef);
    count_alive_pixelsArg3->addAttr(Attribute::ReadOnly);

    BasicBlock *count_alive_pixels_entry_BB =
        BasicBlock::Create(context, "", count_alive_pixelsFunc);
    BasicBlock *count_alive_pixels_BB4 =
        BasicBlock::Create(context, "", count_alive_pixelsFunc);
    BasicBlock *count_alive_pixels_BB13 =
        BasicBlock::Create(context, "", count_alive_pixelsFunc);
    BasicBlock *count_alive_pixels_BB14 =
        BasicBlock::Create(context, "", count_alive_pixelsFunc);
    BasicBlock *count_alive_pixels_BB17 =
        BasicBlock::Create(context, "", count_alive_pixelsFunc);
    BasicBlock *count_alive_pixels_BB20 =
        BasicBlock::Create(context, "", count_alive_pixelsFunc);
    BasicBlock *count_alive_pixels_BB25 =
        BasicBlock::Create(context, "", count_alive_pixelsFunc);
    BasicBlock *count_alive_pixels_BB30 =
        BasicBlock::Create(context, "", count_alive_pixelsFunc);
    BasicBlock *count_alive_pixels_BB35 =
        BasicBlock::Create(context, "", count_alive_pixelsFunc);

    builder.SetInsertPoint(count_alive_pixels_entry_BB);
    //  br label %4
    builder.CreateBr(count_alive_pixels_BB4);
    //
    // 4:                                                ; preds = %3, %14
    builder.SetInsertPoint(count_alive_pixels_BB4);
    //  %5 = phi i64 [ 0, %3 ], [ %15, %14 ]
    PHINode* val5 = builder.CreatePHI(int64Type, 2);
    //  %6 = phi i32 [ 0, %3 ], [ %36, %14 ]
    PHINode* val6 = builder.CreatePHI(int32Type, 2);
    //  %7 = getelementptr inbounds [3 x i32], [3 x i32]* @__const.count_alive_pixels.d, i64 0, i64 %5
    Value* val7 = builder.CreateInBoundsGEP(int32x3ArrayType, d, {builder.getInt64(0), val5});
    //  %8 = load i32, i32* %7, align 4, !tbaa !7
    LoadInst* val8 = builder.CreateLoad(int32Type, val7);
    val8->setAlignment(Align(4));
    //  %9 = add nsw i32 %8, %0
    Value* val9 = builder.CreateAdd(val8, count_alive_pixelsArg1, "", false, true);
    //  %10 = icmp ult i32 %9, 500
    Value* val10 = builder.CreateICmpULT(val9, builder.getInt32(500));
    //  %11 = zext i32 %9 to i64
    Value* val11 = builder.CreateZExt(val9, int64Type);
    //  %12 = icmp ne i64 %5, 1
    Value* val12 = builder.CreateICmpNE(val5, builder.getInt64(1));
    //  br label %17
    builder.CreateBr(count_alive_pixels_BB17);
    //
    // 13:                                               ; preds = %14
    //  ret i32 %36 (находится в BB35)
    //
    // 14:                                               ; preds = %35
    builder.SetInsertPoint(count_alive_pixels_BB14);
    //  %15 = add nuw nsw i64 %5, 1
    Value* val15 = builder.CreateAdd(val5, builder.getInt64(1), "", true, true);
    //  %16 = icmp eq i64 %15, 3
    Value* val16 = builder.CreateICmpEQ(val15, builder.getInt64(3));
    //  br i1 %16, label %13, label %4, !llvm.loop !14
    builder.CreateCondBr(val16, count_alive_pixels_BB13, count_alive_pixels_BB4);
    //
    // 17:                                               ; preds = %4, %35
    builder.SetInsertPoint(count_alive_pixels_BB17);
    //  %18 = phi i64 [ 0, %4 ], [ %37, %35 ]
    PHINode* val18 = builder.CreatePHI(int64Type, 2);
    //  %19 = phi i32 [ %6, %4 ], [ %36, %35 ]
    PHINode* val19 = builder.CreatePHI(int32Type, 2);
    //  br i1 %10, label %20, label %35
    builder.CreateCondBr(val10, count_alive_pixels_BB20, count_alive_pixels_BB35);
    //
    // 20:                                               ; preds = %17
    builder.SetInsertPoint(count_alive_pixels_BB20);
    //  %21 = getelementptr inbounds [3 x i32], [3 x i32]* @__const.count_alive_pixels.d, i64 0, i64 %18
    Value* val21 = builder.CreateInBoundsGEP(int32x3ArrayType, d, {builder.getInt64(0), val18});
    //  %22 = load i32, i32* %21, align 4, !tbaa !7
    LoadInst* val22 = builder.CreateLoad(int32Type, val21);
    val22->setAlignment(Align(4));
    //  %23 = add nsw i32 %22, %1
    Value* val23 = builder.CreateAdd(val22, count_alive_pixelsArg2, "", false, true);
    //  %24 = icmp ult i32 %23, 500
    Value* val24 = builder.CreateICmpULT(val23, builder.getInt32(500));
    //  br i1 %24, label %25, label %35
    builder.CreateCondBr(val24, count_alive_pixels_BB25, count_alive_pixels_BB35);
    //
    // 25:                                               ; preds = %20
    builder.SetInsertPoint(count_alive_pixels_BB25);
    //  %26 = zext i32 %23 to i64
    Value* val26 = builder.CreateZExt(val23, int64Type);
    //  %27 = getelementptr inbounds [500 x i32], [500 x i32]* %2, i64 %11, i64 %26
    Value* val27 = builder.CreateInBoundsGEP(int32ArrayType, count_alive_pixelsArg3, {val11, val26});
    //  %28 = load i32, i32* %27, align 4, !tbaa !7
    LoadInst* val28 = builder.CreateLoad(int32Type, val27);
    val28->setAlignment(Align(4));
    //  %29 = icmp eq i32 %28, 0
    Value* val29 = builder.CreateICmpEQ(val28, builder.getInt32(0));
    //  br i1 %29, label %35, label %30
    builder.CreateCondBr(val29, count_alive_pixels_BB35, count_alive_pixels_BB30);
    //
    // 30:                                               ; preds = %25
    builder.SetInsertPoint(count_alive_pixels_BB30);
    //  %31 = icmp ne i64 %18, 1
    Value* val31 = builder.CreateICmpNE(val18, builder.getInt64(1));
    //  %32 = select i1 %12, i1 true, i1 %31
    Value* val32 = builder.CreateSelect(val12, builder.getTrue(), val31);
    //  %33 = zext i1 %32 to i32
    Value* val33 = builder.CreateZExt(val32, int32Type);
    //  %34 = add nsw i32 %19, %33
    Value* val34 = builder.CreateAdd(val19, val33, "", false, true);
    //  br label %35
    builder.CreateBr(count_alive_pixels_BB35);
    //
    // 35:                                               ; preds = %30, %17, %20, %25
    builder.SetInsertPoint(count_alive_pixels_BB35);
    //  %36 = phi i32 [ %19, %25 ], [ %19, %20 ], [ %19, %17 ], [ %34, %30 ]
    PHINode* val36 = builder.CreatePHI(int32Type, 4);

    builder.SetInsertPoint(count_alive_pixels_BB13);
    builder.CreateRet(val36);

    builder.SetInsertPoint(count_alive_pixels_BB35);
    //  %37 = add nuw nsw i64 %18, 1
    Value* val37 = builder.CreateAdd(val18, builder.getInt64(1), "", true, true);
    //  %38 = icmp eq i64 %37, 3
    Value* val38 = builder.CreateICmpEQ(val37, builder.getInt64(3));
    //  br i1 %38, label %14, label %17, !llvm.loop !15
    builder.CreateCondBr(val38, count_alive_pixels_BB14, count_alive_pixels_BB17);
    //}

    val5->addIncoming(builder.getInt64(0), count_alive_pixels_entry_BB);
    val5->addIncoming(val15, count_alive_pixels_BB14);
    val6->addIncoming(builder.getInt32(0), count_alive_pixels_entry_BB);
    val6->addIncoming(val36, count_alive_pixels_BB14);
    val18->addIncoming(builder.getInt64(0), count_alive_pixels_BB4);
    val18->addIncoming(val37, count_alive_pixels_BB35);
    val19->addIncoming(val6, count_alive_pixels_BB4);
    val19->addIncoming(val36, count_alive_pixels_BB35);
    val36->addIncoming(val19, count_alive_pixels_BB25);
    val36->addIncoming(val19, count_alive_pixels_BB20);
    val36->addIncoming(val19, count_alive_pixels_BB17);
    val36->addIncoming(val34, count_alive_pixels_BB30);
  }
  //; Function Attrs: mustprogress nofree norecurse nosync nounwind optsize uwtable willreturn writeonly
  // define dso_local void @set_new_field_x_y(i32 noundef %0, i32 noundef %1, i32 noundef %2, i32 noundef %3, [500 x i32]* nocapture noundef writeonly %4) {
  ArrayRef<Type *> set_new_field_x_yTypes = {int32Type, int32Type, int32Type, int32Type, int32ArrayPtrType};
  FunctionType *set_new_field_x_yType =
      FunctionType::get(voidType, set_new_field_x_yTypes, false);
  Function *set_new_field_x_yFunc =
      Function::Create(set_new_field_x_yType, Function::ExternalLinkage,
                       "set_new_field_x_y", module.get());
  {
    set_new_field_x_yFunc->addFnAttr(Attribute::MustProgress);
    set_new_field_x_yFunc->addFnAttr(Attribute::NoFree);
    set_new_field_x_yFunc->addFnAttr(Attribute::NoRecurse);
    set_new_field_x_yFunc->addFnAttr(Attribute::NoSync);
    set_new_field_x_yFunc->addFnAttr(Attribute::NoUnwind);
    set_new_field_x_yFunc->addFnAttr(Attribute::OptimizeForSize);
    set_new_field_x_yFunc->addFnAttr(Attribute::UWTable);
    set_new_field_x_yFunc->addFnAttr(Attribute::WillReturn);
    set_new_field_x_yFunc->addFnAttr(Attribute::WriteOnly);

    Argument *set_new_field_x_yArg1 = set_new_field_x_yFunc->getArg(0);
    set_new_field_x_yArg1->addAttr(Attribute::NoUndef);
    Argument *set_new_field_x_yArg2 = set_new_field_x_yFunc->getArg(1);
    set_new_field_x_yArg2->addAttr(Attribute::NoUndef);
    Argument *set_new_field_x_yArg3 = set_new_field_x_yFunc->getArg(2);
    set_new_field_x_yArg3->addAttr(Attribute::NoUndef);
    Argument *set_new_field_x_yArg4 = set_new_field_x_yFunc->getArg(3);
    set_new_field_x_yArg4->addAttr(Attribute::NoUndef);
    Argument *set_new_field_x_yArg5 = set_new_field_x_yFunc->getArg(4);
    set_new_field_x_yArg5->addAttr(Attribute::NoCapture);
    set_new_field_x_yArg5->addAttr(Attribute::NoUndef);
    set_new_field_x_yArg5->addAttr(Attribute::WriteOnly);


    BasicBlock* set_new_field_x_y_entry_BB = BasicBlock::Create(context, "", set_new_field_x_yFunc);
    BasicBlock* set_new_field_x_y_BB7 = BasicBlock::Create(context, "", set_new_field_x_yFunc);
    BasicBlock* set_new_field_x_y_BB13 = BasicBlock::Create(context, "", set_new_field_x_yFunc);
    BasicBlock* set_new_field_x_y_BB14 = BasicBlock::Create(context, "", set_new_field_x_yFunc);
    BasicBlock* set_new_field_x_y_BB15 = BasicBlock::Create(context, "", set_new_field_x_yFunc);
    BasicBlock* set_new_field_x_y_BB20 = BasicBlock::Create(context, "", set_new_field_x_yFunc);
    BasicBlock* set_new_field_x_y_BB21 = BasicBlock::Create(context, "", set_new_field_x_yFunc);
    BasicBlock* set_new_field_x_y_BB22 = BasicBlock::Create(context, "", set_new_field_x_yFunc);

    builder.SetInsertPoint(set_new_field_x_y_entry_BB);
    //  %6 = icmp eq i32 %2, 0
    Value* val6 = builder.CreateICmpEQ(set_new_field_x_yArg3, builder.getInt32(0));
    //  br i1 %6, label %15, label %7
    builder.CreateCondBr(val6, set_new_field_x_y_BB15, set_new_field_x_y_BB7);
    //
    // 7:                                                ; preds = %5
    builder.SetInsertPoint(set_new_field_x_y_BB7);
    //  %8 = and i32 %3, -2
    Value* val8 = builder.CreateAnd(set_new_field_x_yArg4, builder.getInt32(-2));
    //  %9 = icmp eq i32 %8, 2
    Value* val9 = builder.CreateICmpEQ(val8, builder.getInt32(2));
    //  %10 = sext i32 %0 to i64
    Value* val10 = builder.CreateSExt(set_new_field_x_yArg1, int64Type);
    //  %11 = sext i32 %1 to i64
    Value* val11 = builder.CreateSExt(set_new_field_x_yArg2, int64Type);
    //  %12 = getelementptr inbounds [500 x i32], [500 x i32]* %4, i64 %10, i64 %11
    Value* val12 = builder.CreateInBoundsGEP(int32ArrayType, set_new_field_x_yArg5, {val10, val11});
    //  br i1 %9, label %14, label %13
    builder.CreateCondBr(val9, set_new_field_x_y_BB14, set_new_field_x_y_BB13);
    // 13:                                               ; preds = %7
    builder.SetInsertPoint(set_new_field_x_y_BB13);
    //  store i32 0, i32* %12, align 4, !tbaa !7
    StoreInst* storeInst = builder.CreateStore(builder.getInt32(0), val12);
    storeInst->setAlignment(Align(4));
    //  br label %22
    builder.CreateBr(set_new_field_x_y_BB22);
    //
    // 14:                                               ; preds = %7
    builder.SetInsertPoint(set_new_field_x_y_BB14);
    //  store i32 1, i32* %12, align 4, !tbaa !7
    StoreInst* storeInst1 = builder.CreateStore(builder.getInt32(1), val12);
    storeInst1->setAlignment(Align(4));
    //  br label %22
    builder.CreateBr(set_new_field_x_y_BB22);
    //
    // 15:                                               ; preds = %5
    builder.SetInsertPoint(set_new_field_x_y_BB15);
    //  %16 = icmp eq i32 %3, 3
    Value* val16 = builder.CreateICmpEQ(set_new_field_x_yArg4, builder.getInt32(3));
    //  %17 = sext i32 %0 to i64
    Value* val17 = builder.CreateSExt(set_new_field_x_yArg1, int64Type);
    //  %18 = sext i32 %1 to i64
    Value* val18 = builder.CreateSExt(set_new_field_x_yArg2, int64Type);
    //  %19 = getelementptr inbounds [500 x i32], [500 x i32]* %4, i64 %17, i64 %18
    Value* val19 = builder.CreateInBoundsGEP(int32ArrayType, set_new_field_x_yArg5, {val17, val18});
    //  br i1 %16, label %20, label %21
    builder.CreateCondBr(val16, set_new_field_x_y_BB20, set_new_field_x_y_BB21);
    //
    // 20:                                               ; preds = %15
    builder.SetInsertPoint(set_new_field_x_y_BB20);
    //  store i32 1, i32* %19, align 4, !tbaa !7
    StoreInst* storeInst2 = builder.CreateStore(builder.getInt32(1), val19);
    storeInst2->setAlignment(Align(4));
    //  br label %22
    builder.CreateBr(set_new_field_x_y_BB22);
    //
    // 21:                                               ; preds = %15
    builder.SetInsertPoint(set_new_field_x_y_BB21);
    //  store i32 0, i32* %19, align 4, !tbaa !7
    StoreInst* storeInst3 = builder.CreateStore(builder.getInt32(0), val19);
    storeInst3->setAlignment(Align(4));
    //  br label %22
    builder.CreateBr(set_new_field_x_y_BB22);
    //
    // 22:                                               ; preds = %20, %21, %13, %14
    builder.SetInsertPoint(set_new_field_x_y_BB22);
    //  ret void
    builder.CreateRetVoid();
    //}
  }
  //; Function Attrs: nofree norecurse nosync nounwind optsize uwtable
  // define dso_local void @update_field([500 x i32]* nocapture noundef %0, [500 x i32]* nocapture noundef %1) {
  ArrayRef<Type *> update_fieldTypes = {int32ArrayPtrType, int32ArrayPtrType};
  FunctionType *update_fieldType =
      FunctionType::get(voidType, update_fieldTypes, false);
  Function *update_fieldFunc =
      Function::Create(update_fieldType, Function::ExternalLinkage,
                       "update_field", module.get());
  {
    update_fieldFunc->addFnAttr(Attribute::NoFree);
    update_fieldFunc->addFnAttr(Attribute::NoRecurse);
    update_fieldFunc->addFnAttr(Attribute::NoSync);
    update_fieldFunc->addFnAttr(Attribute::NoUnwind);
    update_fieldFunc->addFnAttr(Attribute::OptimizeForSize);
    update_fieldFunc->addFnAttr(Attribute::UWTable);

    Argument *update_fieldArg1 = update_fieldFunc->getArg(0);
    update_fieldArg1->addAttr(Attribute::NoCapture);
    update_fieldArg1->addAttr(Attribute::NoUndef);
    Argument *update_fieldArg2 = update_fieldFunc->getArg(1);
    update_fieldArg2->addAttr(Attribute::NoCapture);
    update_fieldArg2->addAttr(Attribute::NoUndef);


    BasicBlock* update_field_entry_BB = BasicBlock::Create(context, "", update_fieldFunc);
    BasicBlock* update_field_BB3 = BasicBlock::Create(context, "", update_fieldFunc);
    BasicBlock* update_field_BB6 = BasicBlock::Create(context, "", update_fieldFunc);
    BasicBlock* update_field_BB9 = BasicBlock::Create(context, "", update_fieldFunc);
    BasicBlock* update_field_BB16 = BasicBlock::Create(context, "", update_fieldFunc);
    BasicBlock* update_field_BB20 = BasicBlock::Create(context, "", update_fieldFunc);
    BasicBlock* update_field_BB21 = BasicBlock::Create(context, "", update_fieldFunc);
    BasicBlock* update_field_BB22 = BasicBlock::Create(context, "", update_fieldFunc);
    BasicBlock* update_field_BB25 = BasicBlock::Create(context, "", update_fieldFunc);
    BasicBlock* update_field_BB26 = BasicBlock::Create(context, "", update_fieldFunc);
    BasicBlock* update_field_BB27 = BasicBlock::Create(context, "", update_fieldFunc);
    BasicBlock* update_field_BB30 = BasicBlock::Create(context, "", update_fieldFunc);
    BasicBlock* update_field_BB32 = BasicBlock::Create(context, "", update_fieldFunc);
    BasicBlock* update_field_BB33 = BasicBlock::Create(context, "", update_fieldFunc);
    BasicBlock* update_field_BB36 = BasicBlock::Create(context, "", update_fieldFunc);

    builder.SetInsertPoint(update_field_entry_BB);
    //  br label %3
    builder.CreateBr(update_field_BB3);
    //
    // 3:                                                ; preds = %2, %6
    builder.SetInsertPoint(update_field_BB3);
    //  %4 = phi i64 [ 0, %2 ], [ %7, %6 ]
    PHINode* val4 = builder.CreatePHI(int64Type, 2);
    //  %5 = trunc i64 %4 to i32
    Value* val5 = builder.CreateTrunc(val4, int32Type);
    //  br label %9
    builder.CreateBr(update_field_BB9);
    //
    // 6:                                                ; preds = %27
    builder.SetInsertPoint(update_field_BB6);
    //  %7 = add nuw nsw i64 %4, 1
    Value* val7 = builder.CreateAdd(val4, builder.getInt64(1), "", true, true);
    //  %8 = icmp eq i64 %7, 500
    Value* val8 = builder.CreateICmpEQ(val7, builder.getInt64(500));
    //  br i1 %8, label %30, label %3, !llvm.loop !16
    builder.CreateCondBr(val8, update_field_BB30, update_field_BB3);
    //
    // 9:                                                ; preds = %3, %27
    builder.SetInsertPoint(update_field_BB9);
    //  %10 = phi i64 [ 0, %3 ], [ %28, %27 ]
    PHINode* val10 = builder.CreatePHI(int64Type, 2);
    //  %11 = trunc i64 %10 to i32
    Value* val11 = builder.CreateTrunc(val10, int32Type);
    //  %12 = tail call i32 @count_alive_pixels(i32 noundef %5, i32 noundef %11, [500 x i32]* noundef %0)
    CallInst* val12 = builder.CreateCall(count_alive_pixelsFunc, {val5, val11, update_fieldArg1});
    //  %13 = getelementptr inbounds [500 x i32], [500 x i32]* %0, i64 %4, i64 %10
    Value* val13 = builder.CreateInBoundsGEP(int32ArrayType, update_fieldArg1, {val4, val10});
    //  %14 = load i32, i32* %13, align 4, !tbaa !7
    LoadInst* val14 = builder.CreateLoad(int32Type, val13);
    val14->setAlignment(Align(4));
    //  %15 = icmp eq i32 %14, 0
    Value* val15 = builder.CreateICmpEQ(val14, builder.getInt32(0));
    //  br i1 %15, label %22, label %16
    builder.CreateCondBr(val15, update_field_BB22, update_field_BB16);
    //
    // 16:                                               ; preds = %9
    builder.SetInsertPoint(update_field_BB16);
    //  %17 = and i32 %12, -2
    Value* val17 = builder.CreateAnd(val12, builder.getInt32(-2));
    //  %18 = icmp eq i32 %17, 2
    Value* val18 = builder.CreateICmpEQ(val17, builder.getInt32(2));
    //  %19 = getelementptr inbounds [500 x i32], [500 x i32]* %1, i64 %4, i64 %10
    Value* val19 = builder.CreateInBoundsGEP(int32ArrayType, update_fieldArg2, {val4, val10});
    //  br i1 %18, label %21, label %20
    builder.CreateCondBr(val18, update_field_BB21, update_field_BB20);
    //
    // 20:                                               ; preds = %16
    builder.SetInsertPoint(update_field_BB20);
    //  store i32 0, i32* %19, align 4, !tbaa !7
    StoreInst* storeInst = builder.CreateStore(builder.getInt32(0), val19);
    storeInst->setAlignment(Align(4));
    //  br label %27
    builder.CreateBr(update_field_BB27);
    //
    // 21:                                               ; preds = %16
    builder.SetInsertPoint(update_field_BB21);
    //  store i32 1, i32* %19, align 4, !tbaa !7
    StoreInst* storeInst1 = builder.CreateStore(builder.getInt32(1), val19);
    storeInst1->setAlignment(Align(4));
    //  br label %27
    builder.CreateBr(update_field_BB27);
    //
    // 22:                                               ; preds = %9
    builder.SetInsertPoint(update_field_BB22);
    //  %23 = icmp eq i32 %12, 3
    Value* val23 = builder.CreateICmpEQ(val12, builder.getInt32(3));
    //  %24 = getelementptr inbounds [500 x i32], [500 x i32]* %1, i64 %4, i64 %10
    Value* val24 = builder.CreateInBoundsGEP(int32ArrayType, update_fieldArg2,
                                             {val4, val10});
    //  br i1 %23, label %25, label %26
    builder.CreateCondBr(val23, update_field_BB25, update_field_BB26);
    //
    // 25:                                               ; preds = %22
    builder.SetInsertPoint(update_field_BB25);
    //  store i32 1, i32* %24, align 4, !tbaa !7
    StoreInst* storeInst2 = builder.CreateStore(builder.getInt32(1), val24);
    storeInst2->setAlignment(Align(4));
    //  br label %27
    builder.CreateBr(update_field_BB27);
    //
    // 26:                                               ; preds = %22
    builder.SetInsertPoint(update_field_BB26);
    //  store i32 0, i32* %24, align 4, !tbaa !7
    StoreInst* storeInst3 = builder.CreateStore(builder.getInt32(0), val24);
    storeInst3->setAlignment(Align(4));
    //  br label %27
    builder.CreateBr(update_field_BB27);
    //
    // 27:                                               ; preds = %20, %21, %25, %26
    builder.SetInsertPoint(update_field_BB27);
    //  %28 = add nuw nsw i64 %10, 1
    Value* val28 = builder.CreateAdd(val10, builder.getInt64(1), "", true, true);
    //  %29 = icmp eq i64 %28, 500
    Value* val29 = builder.CreateICmpEQ(val28, builder.getInt64(500));
    //  br i1 %29, label %6, label %9, !llvm.loop !17
    builder.CreateCondBr(val29, update_field_BB6, update_field_BB9);
    //
    // 30:                                               ; preds = %6, %33
    builder.SetInsertPoint(update_field_BB30);
    //  %31 = phi i64 [ %34, %33 ], [ 0, %6 ]
    PHINode* val31 = builder.CreatePHI(int64Type, 2);
    //  br label %36
    builder.CreateBr(update_field_BB36);
    //
    // 32:                                               ; preds = %33
    builder.SetInsertPoint(update_field_BB32);
    //  ret void
    builder.CreateRetVoid();
    //
    // 33:                                               ; preds = %36
    builder.SetInsertPoint(update_field_BB33);
    //  %34 = add nuw nsw i64 %31, 1
    Value* val34 = builder.CreateAdd(val31, builder.getInt64(1), "", true, true);
    //  %35 = icmp eq i64 %34, 500
    Value* val35 = builder.CreateICmpEQ(val34, builder.getInt64(500));
    //  br i1 %35, label %32, label %30, !llvm.loop !18
    builder.CreateCondBr(val35, update_field_BB32, update_field_BB30);
    //
    // 36:                                               ; preds = %30, %36
    builder.SetInsertPoint(update_field_BB36);
    //  %37 = phi i64 [ 0, %30 ], [ %41, %36 ]
    PHINode* val37 = builder.CreatePHI(int64Type, 2);
    //  %38 = getelementptr inbounds [500 x i32], [500 x i32]* %1, i64 %31, i64 %37
    Value* val38 = builder.CreateInBoundsGEP(int32ArrayType, update_fieldArg2, {val31, val37});
    //  %39 = load i32, i32* %38, align 4, !tbaa !7
    LoadInst* val39 = builder.CreateLoad(int32Type, val38);
    val39->setAlignment(Align(4));
    //  %40 = getelementptr inbounds [500 x i32], [500 x i32]* %0, i64 %31, i64 %37
    Value* val40 = builder.CreateInBoundsGEP(int32ArrayType, update_fieldArg1, {val31, val37});
    //  store i32 %39, i32* %40, align 4, !tbaa !7
    StoreInst* storeInst4 = builder.CreateStore(val39, val40);
    storeInst4->setAlignment(Align(4));
    //  %41 = add nuw nsw i64 %37, 1
    Value* val41 = builder.CreateAdd(val37, builder.getInt64(1), "", true, true);
    //  %42 = icmp eq i64 %41, 500
    Value* val42 = builder.CreateICmpEQ(val41, builder.getInt64(500));
    //  br i1 %42, label %33, label %36, !llvm.loop !19
    builder.CreateCondBr(val42, update_field_BB33, update_field_BB36);

    val4->addIncoming(builder.getInt64(0), update_field_entry_BB);
    val4->addIncoming(val7, update_field_BB6);
    val10->addIncoming(builder.getInt64(0), update_field_BB3);
    val10->addIncoming(val28, update_field_BB27);
    val31->addIncoming(val34, update_field_BB33);
    val31->addIncoming(builder.getInt64(0), update_field_BB6);
    val37->addIncoming(builder.getInt64(0), update_field_BB30);
    val37->addIncoming(val41, update_field_BB36);
    //}
  }
  //; Function Attrs: nounwind optsize uwtable
  // define void @app() {
  FunctionType *appType =
      FunctionType::get(voidType, {}, false);
  Function *appFunc =
      Function::Create(appType, Function::ExternalLinkage,
                       "app", module.get());
  {
    appFunc->addFnAttr(Attribute::NoReturn);
    appFunc->addFnAttr(Attribute::NoUnwind);
    appFunc->addFnAttr(Attribute::OptimizeForSize);
    appFunc->addFnAttr(Attribute::UWTable);

    BasicBlock* app_entry_BB = BasicBlock::Create(context, "", appFunc);
    BasicBlock* app_BB7 = BasicBlock::Create(context, "", appFunc);

    builder.SetInsertPoint(app_entry_BB);
    //  %1 = alloca [500 x [500 x i32]], align 16
    AllocaInst* val1 = builder.CreateAlloca(int32ArrayArrayType);
    val1->setAlignment(Align(16));
    //  %2 = alloca [500 x [500 x i32]], align 16
    AllocaInst* val2 = builder.CreateAlloca(int32ArrayArrayType);
    val2->setAlignment(Align(16));
    //  %3 = bitcast [500 x [500 x i32]]* %1 to i8*
    Value* val3 = builder.CreateBitCast(val1, int8PtrType);
    //  call void @llvm.lifetime.start.p0i8(i64 1000000, i8* nonnull %3) #8
    builder.CreateLifetimeStart(val3, builder.getInt64(1000000));
    //  %4 = bitcast [500 x [500 x i32]]* %2 to i8*
    Value* val4 = builder.CreateBitCast(val2, int8PtrType);
    //  call void @llvm.lifetime.start.p0i8(i64 1000000, i8* nonnull %4) #8
    builder.CreateLifetimeStart(val4, builder.getInt64(1000000));
    //  %5 = getelementptr inbounds [500 x [500 x i32]], [500 x [500 x i32]]* %1, i64 0, i64 0
    Value* val5 = builder.CreateInBoundsGEP(int32ArrayArrayType, val1, {builder.getInt64(0), builder.getInt64(0)});
    //  call void @fill_init_field([500 x i32]* noundef nonnull %5)
    builder.CreateCall(fill_init_fieldFunc, {val5});
    //  %6 = getelementptr inbounds [500 x [500 x i32]], [500 x [500 x i32]]* %2, i64 0, i64 0
    Value* val6 = builder.CreateInBoundsGEP(int32ArrayArrayType, val2, {builder.getInt64(0), builder.getInt64(0)});
    //  br label %7
    builder.CreateBr(app_BB7);

    // 7:                                                ; preds = %7, %0
    builder.SetInsertPoint(app_BB7);
    //   call void @display_epoch([500 x i32]* noundef nonnull %5) #8
    builder.CreateCall(display_epochFunc, {val5});
    //   tail call void (...) @simFlush() #7
    CallInst* callInst = builder.CreateCall(simFlushFunc);
    callInst->setTailCallKind(CallInst::TCK_Tail);
    //   call void @update_field([500 x i32]* noundef nonnull %5, [500 x i32]* noundef nonnull %6)
    builder.CreateCall(update_fieldFunc, {val5, val6});
    //   br label %7
    builder.CreateBr(app_BB7);
    //}
  }

  // Dump LLVM IR
  module->print(outs(), nullptr);
  outs() << '\n';
  bool verif = verifyModule(*module, &outs());
  outs() << "[VERIFICATION] " << (verif ? "FAIL\n\n" : "OK\n\n");

  // LLVM IR Interpreter
  outs() << "[EE] Run\n";
  InitializeNativeTarget();
  InitializeNativeTargetAsmPrinter();

  ExecutionEngine *ee = EngineBuilder(std::move(module)).create();
  ee->InstallLazyFunctionCreator([](const std::string &fnName) -> void * {
    if (fnName == "simPutPixel") {
      return reinterpret_cast<void *>(simPutPixel);
    }
    if (fnName == "simFlush") {
      return reinterpret_cast<void *>(simFlush);
    }
    if (fnName == "simRand") {
      return reinterpret_cast<void *>(simRand);
    }
    return nullptr;
  });
  ee->finalizeObject();

  simInit();

  ArrayRef<GenericValue> noargs;
  ee->runFunction(appFunc, noargs);

  simExit();
  return EXIT_SUCCESS;
}