; ModuleID = 'fun3.bc'
target datalayout = "e-p:32:32:32-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:32:64-f32:32:32-f64:32:64-v64:64:64-v128:128:128-a0:0:64-f80:32:32-n8:16:32-S128"
target triple = "i386-pc-linux-gnu"

define void @_Z7mathFunfRf(float %x, float* %y) nounwind {
entry:
  %conv = fpext float %x to double
  %call = call double @sin(double %conv) nounwind readnone
  %conv1 = fptrunc double %call to float
  store float %conv1, float* %y, align 4
  ret void
}

declare double @sin(double) nounwind readnone
