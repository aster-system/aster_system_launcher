#include <iostream>
#include "headers/launcher.h"
#include "../scls_files_richard/scls_files.h"

using namespace std;

SCLS_INIT

int main(int argc, char* argv[]) {
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
