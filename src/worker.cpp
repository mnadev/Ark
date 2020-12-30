#include <iostream>
#include <memory>
#include <string>

#include <grpcpp/grpcpp.h>

#include "src/ark.grpc.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using ark::Command;
using ark::Data;
using ark::StorageRequest;
using ark::StorageResponse;
using ark::StorageService;
using ark::StorageStatus;

class ArkWorker: public StorageService::Service {
	public:
    Status requestStorage(ServerContext* context, const StorageRequest* request, StorageResponse* response) {
			Command command = request->command();
			Data data = request->data();

			if (command == Command::ADD) {
					addFile(data);
			}

			return Status::OK;
    }

    void addFile(Data data) {
			std::string file_name = data.file_name();
			std::string file_content = data.value();
    }
};

void run() {
  std::string addr = "0.0.0.0";
  std::string port = "63371";

  ArkWorker ark_worker;

  ServerBuilder builder;
  builder.AddListeningPort(addr + ":" + port, grpc::InsecureServerCredentials()).RegisterService(&ark_worker);
  std::unique_ptr<Server> server(builder.BuildAndStart());
  server->Wait();
}

int main() {
	run();
	return 0;
}
