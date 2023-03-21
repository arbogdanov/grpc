#include <grpcpp/completion_queue.h>
#include <grpcpp/impl/grpc_library.h>
#include <grpcpp/impl/codegen/completion_queue_tag.h>
#include <grpcpp/impl/codegen/grpc_library.h>
#include <grpcpp/notifier.h>

#include "src/core/lib/iomgr/exec_ctx.h"
#include "src/core/lib/surface/completion_queue.h"

namespace grpc_impl {
namespace internal {

class NotifierImpl : public grpc::internal::CompletionQueueTag  {
public:
  NotifierImpl(grpc::CompletionQueue* cq, void* tag)
              : cq_(cq->cq()),
                tag_(tag) {
    GRPC_CQ_INTERNAL_REF(cq_, "notifier");
  }

  ~NotifierImpl() override
  {
    GRPC_CQ_INTERNAL_UNREF(cq_, "notifier");
  }

  bool FinalizeResult(void** tag, bool* /*status*/) override {
    *tag = tag_;
    delete this;
    return true;
  }

  bool Notify() {
    grpc_core::ApplicationCallbackExecCtx callback_exec_ctx;
    grpc_core::ExecCtx exec_ctx;
    if (!grpc_cq_begin_op(cq_, this)) {
      delete this;
      return false;
    }
    grpc_cq_end_op(cq_,
                   this,
                   absl::OkStatus(),
                   [] (void* /*arg*/, grpc_cq_completion* /*completion*/) {},
                   nullptr,
                   &completion_);
    return true;
  }

private:
  grpc_completion_queue* cq_;

  void* tag_;

  grpc_cq_completion completion_;
};

} // namespace internal
} // namespace grpc_impl

namespace grpc {

static grpc::internal::GrpcLibraryInitializer g_gli_initializer;

Notifier::Notifier() {
  g_gli_initializer.summon();
}

bool Notifier::Notify(grpc::CompletionQueue* cq, void* tag) {
  return (new grpc_impl::internal::NotifierImpl(cq, tag))->Notify();
}

} // namespace grpc