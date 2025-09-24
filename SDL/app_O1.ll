; ModuleID = 'app.c'
source_filename = "app.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@__const.count_alive_pixels.d = private unnamed_addr constant [3 x i32] [i32 -1, i32 0, i32 1], align 4

; Function Attrs: nounwind uwtable
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
  %10 = call i32 @simRand() #7
  %11 = and i32 %10, 1
  %12 = icmp eq i32 %11, 0
  br i1 %12, label %19, label %13

13:                                               ; preds = %8
  %14 = call i32 @simRand() #7
  %15 = and i32 %14, 1
  %16 = icmp eq i32 %15, 0
  br i1 %16, label %19, label %17

17:                                               ; preds = %13
  %18 = getelementptr inbounds [500 x i32], [500 x i32]* %0, i64 %3, i64 %9
  store i32 1, i32* %18, align 4, !tbaa !8
  br label %19

19:                                               ; preds = %8, %13, %17
  %20 = add nuw nsw i64 %9, 1
  %21 = icmp eq i64 %20, 500
  br i1 %21, label %5, label %8, !llvm.loop !12
}

; Function Attrs: argmemonly mustprogress nofree nosync nounwind willreturn
declare void @llvm.lifetime.start.p0i8(i64 immarg, i8* nocapture) #1

declare i32 @simRand() local_unnamed_addr #2

; Function Attrs: nounwind uwtable
define dso_local void @display_epoch([500 x i32]* nocapture noundef readonly %0) local_unnamed_addr #0 {
  br label %2

2:                                                ; preds = %1, %6
  %3 = phi i64 [ 0, %1 ], [ %7, %6 ]
  %4 = trunc i64 %3 to i32
  br label %9

5:                                                ; preds = %6
  ret void

6:                                                ; preds = %9
  %7 = add nuw nsw i64 %3, 1
  %8 = icmp eq i64 %7, 500
  br i1 %8, label %5, label %2, !llvm.loop !13

9:                                                ; preds = %2, %9
  %10 = phi i64 [ 0, %2 ], [ %16, %9 ]
  %11 = getelementptr inbounds [500 x i32], [500 x i32]* %0, i64 %3, i64 %10
  %12 = load i32, i32* %11, align 4, !tbaa !8
  %13 = icmp eq i32 %12, 0
  %14 = trunc i64 %10 to i32
  %15 = select i1 %13, i32 0, i32 16777215
  call void @simPutPixel(i32 noundef %4, i32 noundef %14, i32 noundef %15) #7
  %16 = add nuw nsw i64 %10, 1
  %17 = icmp eq i64 %16, 500
  br i1 %17, label %6, label %9, !llvm.loop !14
}

declare void @simPutPixel(i32 noundef, i32 noundef, i32 noundef) local_unnamed_addr #2

; Function Attrs: nofree norecurse nosync nounwind readonly uwtable
define dso_local i32 @count_alive_pixels(i32 noundef %0, i32 noundef %1, [500 x i32]* nocapture noundef readonly %2) local_unnamed_addr #3 {
  br label %4

4:                                                ; preds = %3, %14
  %5 = phi i64 [ 0, %3 ], [ %15, %14 ]
  %6 = phi i32 [ 0, %3 ], [ %36, %14 ]
  %7 = getelementptr inbounds [3 x i32], [3 x i32]* @__const.count_alive_pixels.d, i64 0, i64 %5
  %8 = load i32, i32* %7, align 4, !tbaa !8
  %9 = add nsw i32 %8, %0
  %10 = icmp ult i32 %9, 500
  %11 = sext i32 %9 to i64
  %12 = icmp ne i64 %5, 1
  br label %17

13:                                               ; preds = %14
  ret i32 %36

14:                                               ; preds = %35
  %15 = add nuw nsw i64 %5, 1
  %16 = icmp eq i64 %15, 3
  br i1 %16, label %13, label %4, !llvm.loop !15

17:                                               ; preds = %4, %35
  %18 = phi i64 [ 0, %4 ], [ %37, %35 ]
  %19 = phi i32 [ %6, %4 ], [ %36, %35 ]
  br i1 %10, label %20, label %35

20:                                               ; preds = %17
  %21 = getelementptr inbounds [3 x i32], [3 x i32]* @__const.count_alive_pixels.d, i64 0, i64 %18
  %22 = load i32, i32* %21, align 4, !tbaa !8
  %23 = add nsw i32 %22, %1
  %24 = icmp ult i32 %23, 500
  br i1 %24, label %25, label %35

25:                                               ; preds = %20
  %26 = sext i32 %23 to i64
  %27 = getelementptr inbounds [500 x i32], [500 x i32]* %2, i64 %11, i64 %26
  %28 = load i32, i32* %27, align 4, !tbaa !8
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
  br i1 %38, label %14, label %17, !llvm.loop !16
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind uwtable willreturn writeonly
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
  store i32 0, i32* %12, align 4, !tbaa !8
  br label %22

14:                                               ; preds = %7
  store i32 1, i32* %12, align 4, !tbaa !8
  br label %22

15:                                               ; preds = %5
  %16 = icmp eq i32 %3, 3
  %17 = sext i32 %0 to i64
  %18 = sext i32 %1 to i64
  %19 = getelementptr inbounds [500 x i32], [500 x i32]* %4, i64 %17, i64 %18
  br i1 %16, label %20, label %21

20:                                               ; preds = %15
  store i32 1, i32* %19, align 4, !tbaa !8
  br label %22

21:                                               ; preds = %15
  store i32 0, i32* %19, align 4, !tbaa !8
  br label %22

22:                                               ; preds = %20, %21, %13, %14
  ret void
}

; Function Attrs: nofree norecurse nosync nounwind uwtable
define dso_local void @update_field([500 x i32]* nocapture noundef %0, [500 x i32]* nocapture noundef %1) local_unnamed_addr #5 {
  br label %3

3:                                                ; preds = %2, %9
  %4 = phi i64 [ 0, %2 ], [ %10, %9 ]
  %5 = trunc i64 %4 to i32
  br label %6

6:                                                ; preds = %3, %61
  %7 = phi i64 [ 0, %3 ], [ %62, %61 ]
  %8 = trunc i64 %7 to i32
  br label %12

9:                                                ; preds = %61
  %10 = add nuw nsw i64 %4, 1
  %11 = icmp eq i64 %10, 500
  br i1 %11, label %64, label %3, !llvm.loop !17

12:                                               ; preds = %6, %21
  %13 = phi i64 [ %22, %21 ], [ 0, %6 ]
  %14 = phi i32 [ %43, %21 ], [ 0, %6 ]
  %15 = getelementptr inbounds [3 x i32], [3 x i32]* @__const.count_alive_pixels.d, i64 0, i64 %13
  %16 = load i32, i32* %15, align 4, !tbaa !8
  %17 = add nsw i32 %16, %5
  %18 = icmp ult i32 %17, 500
  %19 = sext i32 %17 to i64
  %20 = icmp ne i64 %13, 1
  br label %24

21:                                               ; preds = %42
  %22 = add nuw nsw i64 %13, 1
  %23 = icmp eq i64 %22, 3
  br i1 %23, label %46, label %12, !llvm.loop !15

24:                                               ; preds = %42, %12
  %25 = phi i64 [ 0, %12 ], [ %44, %42 ]
  %26 = phi i32 [ %14, %12 ], [ %43, %42 ]
  br i1 %18, label %27, label %42

27:                                               ; preds = %24
  %28 = getelementptr inbounds [3 x i32], [3 x i32]* @__const.count_alive_pixels.d, i64 0, i64 %25
  %29 = load i32, i32* %28, align 4, !tbaa !8
  %30 = add nsw i32 %29, %8
  %31 = icmp ult i32 %30, 500
  br i1 %31, label %32, label %42

32:                                               ; preds = %27
  %33 = sext i32 %30 to i64
  %34 = getelementptr inbounds [500 x i32], [500 x i32]* %0, i64 %19, i64 %33
  %35 = load i32, i32* %34, align 4, !tbaa !8
  %36 = icmp eq i32 %35, 0
  br i1 %36, label %42, label %37

37:                                               ; preds = %32
  %38 = icmp ne i64 %25, 1
  %39 = select i1 %20, i1 true, i1 %38
  %40 = zext i1 %39 to i32
  %41 = add nsw i32 %26, %40
  br label %42

42:                                               ; preds = %37, %32, %27, %24
  %43 = phi i32 [ %26, %32 ], [ %26, %27 ], [ %26, %24 ], [ %41, %37 ]
  %44 = add nuw nsw i64 %25, 1
  %45 = icmp eq i64 %44, 3
  br i1 %45, label %21, label %24, !llvm.loop !16

46:                                               ; preds = %21
  %47 = getelementptr inbounds [500 x i32], [500 x i32]* %0, i64 %4, i64 %7
  %48 = load i32, i32* %47, align 4, !tbaa !8
  %49 = icmp eq i32 %48, 0
  br i1 %49, label %56, label %50

50:                                               ; preds = %46
  %51 = and i32 %43, -2
  %52 = icmp eq i32 %51, 2
  %53 = getelementptr inbounds [500 x i32], [500 x i32]* %1, i64 %4, i64 %7
  br i1 %52, label %55, label %54

54:                                               ; preds = %50
  store i32 0, i32* %53, align 4, !tbaa !8
  br label %61

55:                                               ; preds = %50
  store i32 1, i32* %53, align 4, !tbaa !8
  br label %61

56:                                               ; preds = %46
  %57 = icmp eq i32 %43, 3
  %58 = getelementptr inbounds [500 x i32], [500 x i32]* %1, i64 %4, i64 %7
  br i1 %57, label %59, label %60

59:                                               ; preds = %56
  store i32 1, i32* %58, align 4, !tbaa !8
  br label %61

60:                                               ; preds = %56
  store i32 0, i32* %58, align 4, !tbaa !8
  br label %61

61:                                               ; preds = %54, %55, %59, %60
  %62 = add nuw nsw i64 %7, 1
  %63 = icmp eq i64 %62, 500
  br i1 %63, label %9, label %6, !llvm.loop !18

64:                                               ; preds = %9, %67
  %65 = phi i64 [ %68, %67 ], [ 0, %9 ]
  br label %70

66:                                               ; preds = %67
  ret void

67:                                               ; preds = %70
  %68 = add nuw nsw i64 %65, 1
  %69 = icmp eq i64 %68, 500
  br i1 %69, label %66, label %64, !llvm.loop !19

70:                                               ; preds = %64, %70
  %71 = phi i64 [ 0, %64 ], [ %75, %70 ]
  %72 = getelementptr inbounds [500 x i32], [500 x i32]* %1, i64 %65, i64 %71
  %73 = load i32, i32* %72, align 4, !tbaa !8
  %74 = getelementptr inbounds [500 x i32], [500 x i32]* %0, i64 %65, i64 %71
  store i32 %73, i32* %74, align 4, !tbaa !8
  %75 = add nuw nsw i64 %71, 1
  %76 = icmp eq i64 %75, 500
  br i1 %76, label %67, label %70, !llvm.loop !20
}

; Function Attrs: noreturn nounwind uwtable
define dso_local void @app() local_unnamed_addr #6 {
  %1 = alloca [500 x [500 x i32]], align 16
  %2 = alloca [500 x [500 x i32]], align 16
  %3 = bitcast [500 x [500 x i32]]* %1 to i8*
  call void @llvm.lifetime.start.p0i8(i64 1000000, i8* nonnull %3) #7
  %4 = bitcast [500 x [500 x i32]]* %2 to i8*
  call void @llvm.lifetime.start.p0i8(i64 1000000, i8* nonnull %4) #7
  br label %5

5:                                                ; preds = %7, %0
  %6 = phi i64 [ 0, %0 ], [ %8, %7 ]
  br label %13

7:                                                ; preds = %24
  %8 = add nuw nsw i64 %6, 1
  %9 = icmp eq i64 %8, 500
  br i1 %9, label %10, label %5, !llvm.loop !5

10:                                               ; preds = %7
  %11 = getelementptr inbounds [500 x [500 x i32]], [500 x [500 x i32]]* %1, i64 0, i64 0
  %12 = getelementptr inbounds [500 x [500 x i32]], [500 x [500 x i32]]* %2, i64 0, i64 0
  br label %27

13:                                               ; preds = %24, %5
  %14 = phi i64 [ 0, %5 ], [ %25, %24 ]
  %15 = call i32 @simRand() #7
  %16 = and i32 %15, 1
  %17 = icmp eq i32 %16, 0
  br i1 %17, label %24, label %18

18:                                               ; preds = %13
  %19 = call i32 @simRand() #7
  %20 = and i32 %19, 1
  %21 = icmp eq i32 %20, 0
  br i1 %21, label %24, label %22

22:                                               ; preds = %18
  %23 = getelementptr inbounds [500 x [500 x i32]], [500 x [500 x i32]]* %1, i64 0, i64 %6, i64 %14
  store i32 1, i32* %23, align 4, !tbaa !8
  br label %24

24:                                               ; preds = %22, %18, %13
  %25 = add nuw nsw i64 %14, 1
  %26 = icmp eq i64 %25, 500
  br i1 %26, label %7, label %13, !llvm.loop !12

27:                                               ; preds = %34, %10
  %28 = phi i64 [ 0, %10 ], [ %35, %34 ]
  %29 = trunc i64 %28 to i32
  %30 = trunc i64 %28 to i32
  br label %36

31:                                               ; preds = %44
  %32 = add nuw nsw i64 %28, 1
  %33 = icmp eq i64 %32, 500
  br i1 %33, label %47, label %34

34:                                               ; preds = %31, %47
  %35 = phi i64 [ %32, %31 ], [ 0, %47 ]
  br label %27, !llvm.loop !21

36:                                               ; preds = %44, %27
  %37 = phi i64 [ 0, %27 ], [ %45, %44 ]
  %38 = getelementptr inbounds [500 x [500 x i32]], [500 x [500 x i32]]* %1, i64 0, i64 %28, i64 %37
  %39 = load i32, i32* %38, align 4, !tbaa !8
  %40 = icmp eq i32 %39, 0
  %41 = trunc i64 %37 to i32
  br i1 %40, label %43, label %42

42:                                               ; preds = %36
  call void @simPutPixel(i32 noundef %29, i32 noundef %41, i32 noundef 16777215) #7
  br label %44

43:                                               ; preds = %36
  call void @simPutPixel(i32 noundef %30, i32 noundef %41, i32 noundef 0) #7
  br label %44

44:                                               ; preds = %43, %42
  %45 = add nuw nsw i64 %37, 1
  %46 = icmp eq i64 %45, 500
  br i1 %46, label %31, label %36, !llvm.loop !14

47:                                               ; preds = %31
  call void (...) @simFlush() #7
  call void @update_field([500 x i32]* noundef nonnull %11, [500 x i32]* noundef nonnull %12)
  br label %34
}

declare void @simFlush(...) local_unnamed_addr #2

attributes #0 = { nounwind uwtable "frame-pointer"="none" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { argmemonly mustprogress nofree nosync nounwind willreturn }
attributes #2 = { "frame-pointer"="none" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #3 = { nofree norecurse nosync nounwind readonly uwtable "frame-pointer"="none" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #4 = { mustprogress nofree norecurse nosync nounwind uwtable willreturn writeonly "frame-pointer"="none" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #5 = { nofree norecurse nosync nounwind uwtable "frame-pointer"="none" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #6 = { noreturn nounwind uwtable "frame-pointer"="none" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #7 = { nounwind }

!llvm.module.flags = !{!0, !1, !2, !3}
!llvm.ident = !{!4}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 7, !"PIC Level", i32 2}
!2 = !{i32 7, !"PIE Level", i32 2}
!3 = !{i32 7, !"uwtable", i32 1}
!4 = !{!"Ubuntu clang version 14.0.0-1ubuntu1.1"}
!5 = distinct !{!5, !6, !7}
!6 = !{!"llvm.loop.mustprogress"}
!7 = !{!"llvm.loop.unroll.disable"}
!8 = !{!9, !9, i64 0}
!9 = !{!"int", !10, i64 0}
!10 = !{!"omnipotent char", !11, i64 0}
!11 = !{!"Simple C/C++ TBAA"}
!12 = distinct !{!12, !6, !7}
!13 = distinct !{!13, !6, !7}
!14 = distinct !{!14, !6, !7}
!15 = distinct !{!15, !6, !7}
!16 = distinct !{!16, !6, !7}
!17 = distinct !{!17, !6, !7}
!18 = distinct !{!18, !6, !7}
!19 = distinct !{!19, !6, !7}
!20 = distinct !{!20, !6, !7}
!21 = distinct !{!21, !7}
