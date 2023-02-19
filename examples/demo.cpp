#include "ecas/ecas.hpp"

#include <thread>

void TaskA(std::vector<ecas::ITensor *> &input, std::vector<ecas::ITensor *> &output) {
    static int count = 0;
    printf("TaskA: %d (%d).\n", count++, std::this_thread::get_id());
}
void TaskB(std::vector<ecas::ITensor *> &input, std::vector<ecas::ITensor *> &output) {
    static int count = 0;
    printf("TaskB: %d (%d).\n", count++, std::this_thread::get_id());
}
void TaskC(std::vector<ecas::ITensor *> &input, std::vector<ecas::ITensor *> &output) {
    static int count = 0;
    printf("TaskC: %d (%d).\n", count++, std::this_thread::get_id());
}
void TaskD(std::vector<ecas::ITensor *> &input, std::vector<ecas::ITensor *> &output) {
    static int count = 0;
    printf("TaskD: %d (%d).\n", count++, std::this_thread::get_id());
}

int main() {
    ecas::HelloWorld();
    // ecas::VulkanHelloWorld();
    
    ecas::SessionConfig config;
    config.mode = ecas::ExecutionMode::SINGLE;
    config.num_thread = 1;
    ecas::Session *session = new ecas::Session("s1", config);

    // TODO: 简化API
    session->CreateNode("n1", TaskA, {{2}}, {{2, 2}, {3, 3}}, 0);
    session->CreateNode("n2", TaskB, {{2, 2}}, {{2, 2}}, 1);
    session->CreateNode("n3", TaskC, {{3, 3}}, {{3, 3}}, 0);
    session->CreateNode("n4", TaskD, {{2, 2}, {3, 3}}, {{2}}, 0);
    session->CreateNode("n5", {{"n1", "n2"}, {"n2", "n3"}});
    
    session->BuildGraph({{"n1", "n2"}, {"n1", "n3"}, {"n2", "n4"}, {"n3", "n4"}});
    // session->Group({{"n1"}, {"n2", "n3"}});
    // session->Group({{"n1", "n2", "n3"}});
    session->ShowInfo();

    ecas::ITensor *in = session->CreateITensor({2});
    in->data[0] = 1; in->data[1] = 2;
    ecas::ITensor *out = session->CreateITensor({2});

    session->Start();
    for (int i=0; i<5; i++) {
        in->id = i;
        session->GraphFeed(in);
    }
    for (int i=0; i<5; i++) {
        session->GraphGetResult(out);
        printf("out id: %d.\n", out->id);
    }
    std::this_thread::sleep_for(std::chrono::seconds(2));
    printf("Call stop.\n");
    session->Stop();
    return 0;
}