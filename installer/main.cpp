#include <iostream>
#include "../../scls_files_richard/scls_files.h"
#include "../../scls_network_tim/scls_network.h"

using namespace std;

SCLS_INIT

// Datas of the server
std::string __server_ip = std::string();int __server_port;

int main(int argc, char* argv[]) {
    // Basic datas
    std::string filename = std::string("docs.zip");
    std::string final_path = std::string();
    std::string operation = std::string("0");
    if(argc >= 2){operation = argv[1];}
    if(argc >= 3){final_path = argv[2];}
    if(argc >= 4){filename = argv[3];}

    if(operation == std::string("0")) {
        // Install a software

        // Load the server
        if(std::filesystem::exists("../.env")) {
            std::shared_ptr<scls::__Balise_Container> balises = std::make_shared<scls::__Balise_Container>();
            std::shared_ptr<scls::XML_Text> configuration_datas = scls::xml(balises, scls::read_file("../.env"));

            // Get the datas about the server
            scls::XML_Text* server_datas = configuration_datas.get()->balise_by_name("server");
            std::vector<scls::XML_Attribute>& attributes = server_datas->xml_balise_attributes();
            for(int j = 0;j<static_cast<int>(attributes.size());j++) {
                scls::XML_Attribute& current_attribute = attributes.at(j);
                std::string current_attribute_name = current_attribute.name;
                std::string current_attribute_value = current_attribute.value;
                if(current_attribute_name == "ip") {__server_ip = current_attribute_value;}
                else if(current_attribute_name == "port") {__server_port = std::stoi(current_attribute_value);}
            }
        }

        // Path to copy the software
        std::string final_path = scls::program_directory() + std::string("Aster_System\\");

        // Get the software
        if(!std::filesystem::exists(final_path)){std::filesystem::create_directory(final_path);}
        scls::FTP_Client ftp = scls::FTP_Client(__server_ip);
        int result = ftp.connect();if(result < 0){scls::write_in_file(std::string("result.txt"), std::string("<error return=\"-1\">"));return -1;}
        ftp.retrieve(filename).datas.get()->save(final_path + std::string("temp.zip"));;
        ftp.disconnect();
        //*/

        // Extract the datas
        scls::ZIP_File file = scls::ZIP_File(final_path + std::string("temp.zip"));
        file.extract_zip(final_path);
        if(std::filesystem::exists(final_path + std::string("temp.zip"))){std::filesystem::remove(final_path + std::string("temp.zip"));}

        // Get the execution path
        std::string execution_path = std::string();
        for(int i = 0;i<static_cast<int>(file.local_files().size());i++) {
            std::vector<std::string> cutted = scls::cut_string(file.local_files().at(i).get()->file_name, std::string("."));
            if(cutted.size() > 1 && cutted.at(cutted.size() - 1) == std::string("exe")){
                execution_path = file.local_files().at(i).get()->file_name;
                cutted = scls::cut_string(scls::replace(execution_path, std::string("\\"), std::string("/")), std::string("/"));
                execution_path = std::string();
                for(int i = 1;i<static_cast<int>(cutted.size());i++){execution_path += cutted.at(i);if(i != static_cast<int>(cutted.size()) - 1){execution_path += std::string("/");}}
                break;
            }
        }

        // Finish the installation
        scls::write_in_file(std::string("result.txt"), std::string("<software installation_path=\"") + final_path + file.root() + std::string("\\\" execution_path=\"") + execution_path + std::string("\">"));
    }
    else if(operation == std::string("1")) {
        // Uninstall a software
        std::filesystem::remove_all(final_path);

        // Finish the uninstallation
        scls::write_in_file(std::string("result.txt"), std::string("Good"));
    }

    return 0;
}
