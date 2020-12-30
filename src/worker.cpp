#include <iostream>
#include <fstream>
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

			std::string file_name = data.file_name();
			std::string file_content = data.value();

			if (command == Command::ADD) {
				addFile(file_name, file_content);
			} else if (command == Command::APPEND) {
				appendFile(file_name, file_content);
			} else if (command == Command::DELETE) {
				deleteFile(file_name);
			} else if (command == Command::CLEAR) {
				clearFile(file_name);
			} else {
				return Status::CANCELLED;
			}

			return Status::OK;
    }

	private:
		void addFile(std::string file_name, std::string file_content) {
			std::ofstream file;
			file.open(file_name, std::ofstream::out);
			file << file_content << std::endl;
			file.close();
		}

		void appendFile(std::string file_name, std::string file_content) {
			std::ofstream file;
			file.open(file_name, std::ofstream::out | std::ofstream::app);
			file << file_content << std::endl;
			file.close();
		}

		void clearFile(std::string file_name) {
			std::ofstream file;
			file.open(file_name, std::ofstream::out | std::ofstream::trunc);
			file.close();
		}

		void deleteFile(std::string file_name) {
			std::remove(file_name.c_str());
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
