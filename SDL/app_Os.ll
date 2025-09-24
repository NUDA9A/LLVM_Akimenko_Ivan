; ModuleID = 'app.c'
source_filename = "app.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@__const.count_alive_pixels.d = private unnamed_addr constant [3 x i32] [i32 -1, i32 0, i32 1], align 4

; Function Attrs: nounwind optsize uwtable
define dso_local void @fill_init_field([500 x i32]* nocapture noundef writeonly %0) local_unnamed_addr #0 {
  br label %2

2:                                                ; preds = %1, %5
  %3 = phi i64 [ 0, %1 ], [ %6, %5 ]
  br label %8

4:                                                ; preds = %5
  ret void

5:                                                ; preds = %19
  %6 = add nuw nsw i64 %3, 1
  %7 = icmp eq i64 %6, 500
  br i1 %7, label %4, label %2, !llvm.loop !5

8:                                                ; preds = %2, %19
  %9 = phi i64 [ 0, %2 ], [ %20, %19 ]
  %10 = tail call i32 @simRand() #7
  %11 = and i32 %10, 1
  %12 = icmp eq i32 %11, 0
  br i1 %12, label %19, label %13

13:                                               ; preds = %8
  %14 = tail call i32 @simRand() #7
  %15 = and i32 %14, 1
  %16 = icmp eq i32 %15, 0
  br i1 %16, label %19, label %17

17:                                               ; preds = %13
  %18 = getelementptr inbounds [500 x i32], [500 x i32]* %0, i64 %3, i64 %9
  store i32 1, i32* %18, align 4, !tbaa !7
  br label %19

19:                                               ; preds = %8, %13, %17
  %20 = add nuw nsw i64 %9, 1
  %21 = icmp eq i64 %20, 500
  br i1 %21, label %5, label %8, !llvm.loop !11
}

; Function Attrs: argmemonly mustprogress nofree nosync nounwind willreturn
declare void @llvm.lifetime.start.p0i8(i64 immarg, i8* nocapture) #1

; Function Attrs: optsize
declare i32 @simRand() local_unnamed_addr #2

; Function Attrs: nounwind optsize uwtable
define dso_local void @display_epoch([500 x i32]* nocapture noundef readonly %0) local_unnamed_addr #0 {
  br label %2

2:                                                ; preds = %1, %7
  %3 = phi i64 [ 0, %1 ], [ %8, %7 ]
  %4 = trunc i64 %3 to i32
  %5 = trunc i64 %3 to i32
  br label %10

6:                                                ; preds = %7
  ret void

7:                                                ; preds = %18
  %8 = add nuw nsw i64 %3, 1
  %9 = icmp eq i64 %8, 500
  br i1 %9, label %6, label %2, !llvm.loop !12

10:                                               ; preds = %2, %18
  %11 = phi i64 [ 0, %2 ], [ %19, %18 ]
  %12 = getelementptr inbounds [500 x i32], [500 x i32]* %0, i64 %3, i64 %11
  %13 = load i32, i32* %12, align 4, !tbaa !7
  %14 = icmp eq i32 %13, 0
  %15 = trunc i64 %11 to i32
  br i1 %14, label %17, label %16

16:                                               ; preds = %10
  tail call void @simPutPixel(i32 noundef %4, i32 noundef %15, i32 noundef 16777215) #7
  br label %18

17:                                               ; preds = %10
  tail call void @simPutPixel(i32 noundef %5, i32 noundef %15, i32 noundef 0) #7
  br label %18

18:                                               ; preds = %16, %17
  %19 = add nuw nsw i64 %11, 1
  %20 = icmp eq i64 %19, 500
  br i1 %20, label %7, label %10, !llvm.loop !13
}

; Function Attrs: optsize
declare void @simPutPixel(i32 noundef, i32 noundef, i32 noundef) local_unnamed_addr #2

; Function Attrs: nofree norecurse nosync nounwind optsize readonly uwtable
define dso_local i32 @count_alive_pixels(i32 noundef %0, i32 noundef %1, [500 x i32]* nocapture noundef readonly %2) local_unnamed_addr #3 {
  br label %4

4:                                                ; preds = %3, %14
  %5 = phi i64 [ 0, %3 ], [ %15, %14 ]
  %6 = phi i32 [ 0, %3 ], [ %36, %14 ]
  %7 = getelementptr inbounds [3 x i32], [3 x i32]* @__const.count_alive_pixels.d, i64 0, i64 %5
  %8 = load i32, i32* %7, align 4, !tbaa !7
  %9 = add nsw i32 %8, %0
  %10 = icmp ult i32 %9, 500
  %11 = zext i32 %9 to i64
  %12 = icmp ne i64 %5, 1
  br label %17

13:                                               ; preds = %14
  ret i32 %36

14:                                               ; preds = %35
  %15 = add nuw nsw i64 %5, 1
  %16 = icmp eq i64 %15, 3
  br i1 %16, label %13, label %4, !llvm.loop !14

17:                                               ; preds = %4, %35
  %18 = phi i64 [ 0, %4 ], [ %37, %35 ]
  %19 = phi i32 [ %6, %4 ], [ %36, %35 ]
  br i1 %10, label %20, label %35

20:                                               ; preds = %17
  %21 = getelementptr inbounds [3 x i32], [3 x i32]* @__const.count_alive_pixels.d, i64 0, i64 %18
  %22 = load i32, i32* %21, align 4, !tbaa !7
  %23 = add nsw i32 %22, %1
  %24 = icmp ult i32 %23, 500
  br i1 %24, label %25, label %35

25:                                               ; preds = %20
  %26 = zext i32 %23 to i64
  %27 = getelementptr inbounds [500 x i32], [500 x i32]* %2, i64 %11, i64 %26
  %28 = load i32, i32* %27, align 4, !tbaa !7
  %29 = icmp eq i32 %28, 0
  br i1 %29, label %35, label %30

30:                                               ; preds = %25
  %31 = icmp ne i64 %18, 1
  %32 = select i1 %12, i1 true, i1 %31
  %33 = zext i1 %32 to i32
  %34 = add nsw i32 %19, %33
  br label %35

35:                                               ; preds = %30, %17, %20, %25
  %36 = phi i32 [ %19, %25 ], [ %19, %20 ], [ %19, %17 ], [ %34, %30 ]
  %37 = add nuw nsw i64 %18, 1
  %38 = icmp eq i64 %37, 3
  br i1 %38, label %14, label %17, !llvm.loop !15
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind optsize uwtable willreturn writeonly
define dso_local void @set_new_field_x_y(i32 noundef %0, i32 noundef %1, i32 noundef %2, i32 noundef %3, [500 x i32]* nocapture noundef writeonly %4) local_unnamed_addr #4 {
  %6 = icmp eq i32 %2, 0
  br i1 %6, label %15, label %7

7:                                                ; preds = %5
  %8 = and i32 %3, -2
  %9 = icmp eq i32 %8, 2
  %10 = sext i32 %0 to i64
  %11 = sext i32 %1 to i64
  %12 = getelementptr inbounds [500 x i32], [500 x i32]* %4, i64 %10, i64 %11
  br i1 %9, label %14, label %13

13:                                               ; preds = %7
  store i32 0, i32* %12, align 4, !tbaa !7
  br label %22

14:                                               ; preds = %7
  store i32 1, i32* %12, align 4, !tbaa !7
  br label %22

15:                                               ; preds = %5
  %16 = icmp eq i32 %3, 3
  %17 = sext i32 %0 to i64
  %18 = sext i32 %1 to i64
  %19 = getelementptr inbounds [500 x i32], [500 x i32]* %4, i64 %17, i64 %18
  br i1 %16, label %20, label %21

20:                                               ; preds = %15
  store i32 1, i32* %19, align 4, !tbaa !7
  br label %22

21:                                               ; preds = %15
  store i32 0, i32* %19, align 4, !tbaa !7
  br label %22

22:                                               ; preds = %20, %21, %13, %14
  ret void
}

; Function Attrs: nofree norecurse nosync nounwind optsize uwtable
define dso_local void @update_field([500 x i32]* nocapture noundef %0, [500 x i32]* nocapture noundef %1) local_unnamed_addr #5 {
  br label %3

3:                                                ; preds = %2, %6
  %4 = phi i64 [ 0, %2 ], [ %7, %6 ]
  %5 = trunc i64 %4 to i32
  br label %9

6:                                                ; preds = %27
  %7 = add nuw nsw i64 %4, 1
  %8 = icmp eq i64 %7, 500
  br i1 %8, label %30, label %3, !llvm.loop !16

9:                                                ; preds = %3, %27
  %10 = phi i64 [ 0, %3 ], [ %28, %27 ]
  %11 = trunc i64 %10 to i32
  %12 = tail call i32 @count_alive_pixels(i32 noundef %5, i32 noundef %11, [500 x i32]* noundef %0) #8
  %13 = getelementptr inbounds [500 x i32], [500 x i32]* %0, i64 %4, i64 %10
  %14 = load i32, i32* %13, align 4, !tbaa !7
  %15 = icmp eq i32 %14, 0
  br i1 %15, label %22, label %16

16:                                               ; preds = %9
  %17 = and i32 %12, -2
  %18 = icmp eq i32 %17, 2
  %19 = getelementptr inbounds [500 x i32], [500 x i32]* %1, i64 %4, i64 %10
  br i1 %18, label %21, label %20

20:                                               ; preds = %16
  store i32 0, i32* %19, align 4, !tbaa !7
  br label %27

21:                                               ; preds = %16
  store i32 1, i32* %19, align 4, !tbaa !7
  br label %27

22:                                               ; preds = %9
  %23 = icmp eq i32 %12, 3
  %24 = getelementptr inbounds [500 x i32], [500 x i32]* %1, i64 %4, i64 %10
  br i1 %23, label %25, label %26

25:                                               ; preds = %22
  store i32 1, i32* %24, align 4, !tbaa !7
  br label %27

26:                                               ; preds = %22
  store i32 0, i32* %24, align 4, !tbaa !7
  br label %27

27:                                               ; preds = %20, %21, %25, %26
  %28 = add nuw nsw i64 %10, 1
  %29 = icmp eq i64 %28, 500
  br i1 %29, label %6, label %9, !llvm.loop !17

30:                                               ; preds = %6, %33
  %31 = phi i64 [ %34, %33 ], [ 0, %6 ]
  br label %36

32:                                               ; preds = %33
  ret void

33:                                               ; preds = %36
  %34 = add nuw nsw i64 %31, 1
  %35 = icmp eq i64 %34, 500
  br i1 %35, label %32, label %30, !llvm.loop !18

36:                                               ; preds = %30, %36
  %37 = phi i64 [ 0, %30 ], [ %41, %36 ]
  %38 = getelementptr inbounds [500 x i32], [500 x i32]* %1, i64 %31, i64 %37
  %39 = load i32, i32* %38, align 4, !tbaa !7
  %40 = getelementptr inbounds [500 x i32], [500 x i32]* %0, i64 %31, i64 %37
  store i32 %39, i32* %40, align 4, !tbaa !7
  %41 = add nuw nsw i64 %37, 1
  %42 = icmp eq i64 %41, 500
  br i1 %42, label %33, label %36, !llvm.loop !19
}

; Function Attrs: noreturn nounwind optsize uwtable
define dso_local void @app() local_unnamed_addr #6 {
  %1 = alloca [500 x [500 x i32]], align 16
  %2 = alloca [500 x [500 x i32]], align 16
  %3 = bitcast [500 x [500 x i32]]* %1 to i8*
  call void @llvm.lifetime.start.p0i8(i64 1000000, i8* nonnull %3) #9
  %4 = bitcast [500 x [500 x i32]]* %2 to i8*
  call void @llvm.lifetime.start.p0i8(i64 1000000, i8* nonnull %4) #9
  %5 = getelementptr inbounds [500 x [500 x i32]], [500 x [500 x i32]]* %1, i64 0, i64 0
  call void @fill_init_field([500 x i32]* noundef nonnull %5) #8
  %6 = getelementptr inbounds [500 x [500 x i32]], [500 x [500 x i32]]* %2, i64 0, i64 0
  br label %7

7:                                                ; preds = %7, %0
  call void @display_epoch([500 x i32]* noundef nonnull %5) #8
  tail call void (...) @simFlush() #7
  call void @update_field([500 x i32]* noundef nonnull %5, [500 x i32]* noundef nonnull %6) #8
  br label %7
}

; Function Attrs: optsize
declare void @simFlush(...) local_unnamed_addr #2

attributes #0 = { nounwind optsize uwtable "frame-pointer"="none" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { argmemonly mustprogress nofree nosync nounwind willreturn }
attributes #2 = { optsize "frame-pointer"="none" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #3 = { nofree norecurse nosync nounwind optsize readonly uwtable "frame-pointer"="none" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #4 = { mustprogress nofree norecurse nosync nounwind optsize uwtable willreturn writeonly "frame-pointer"="none" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #5 = { nofree norecurse nosync nounwind optsize uwtable "frame-pointer"="none" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #6 = { noreturn nounwind optsize uwtable "frame-pointer"="none" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #7 = { nounwind optsize }
attributes #8 = { optsize }
attributes #9 = { nounwind }

!llvm.module.flags = !{!0, !1, !2, !3}
!llvm.ident = !{!4}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 7, !"PIC Level", i32 2}
!2 = !{i32 7, !"PIE Level", i32 2}
!3 = !{i32 7, !"uwtable", i32 1}
!4 = !{!"Ubuntu clang version 14.0.0-1ubuntu1.1"}
!5 = distinct !{!5, !6}
!6 = !{!"llvm.loop.mustprogress"}
!7 = !{!8, !8, i64 0}
!8 = !{!"int", !9, i64 0}
!9 = !{!"omnipotent char", !10, i64 0}
!10 = !{!"Simple C/C++ TBAA"}
!11 = distinct !{!11, !6}
!12 = distinct !{!12, !6}
!13 = distinct !{!13, !6}
!14 = distinct !{!14, !6}
!15 = distinct !{!15, !6}
!16 = distinct !{!16, !6}
!17 = distinct !{!17, !6}
!18 = distinct !{!18, !6}
!19 = distinct !{!19, !6}
