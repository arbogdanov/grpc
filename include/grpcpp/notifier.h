#ifndef GRPCPP_NOTIFIER_H
#define GRPCPP_NOTIFIER_H

#include <memory>

#include <grpc/grpc.h>
#include <grpcpp/completion_queue.h>

namespace grpc {

class Notifier : private grpc::GrpcLibraryCodegen {
public:
  Notifier();

  ~Notifier() override = default;

  bool Notify(grpc::CompletionQueue* cq, void* tag);
};

} // namespace grpc

#endif  // GRPCPP_NOTIFIER_H
