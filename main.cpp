#include <iostream>
#include "headers/launcher.h"
#include "../scls_files_richard/scls_files.h"

using namespace std;

SCLS_INIT

int main(int argc, char* argv[]) {
    scls::Socket socket = scls::Socket("tarn.mon-ent-occitanie.fr", 80);
    int result = socket.connect();
    socket.send_datas("GET / HTTP/1.1\r\nConnection: close\r\nHost: tarn.mon-ent-occitanie.fr\r\n\r\n");
    std::cout << "E " << socket.receive_datas().datas.get()->extract_string_all() << std::endl;

    /*
    as_launcher::AS_Window window(900, 600, argv[0]);
    window.load_from_xml("assets/window.txt");

    while(window.run()) {
        window.update_event();
        window.update();

        window.render();
    }
    //*/

    return 0;
}
