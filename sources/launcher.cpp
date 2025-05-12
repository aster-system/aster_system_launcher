//******************
//
// launcher.h
//
//******************
// Presentation :
//
// This file contains the source code of the launcher.
//
//******************
//
// License (LGPL V3.0) :
//
// Copyright (C) 2024 by Aster System, Inc. <https://aster-system.github.io/aster-system/>
// This file is part of SCLS.
// SCLS is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// SCLS is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
// You should have received a copy of the GNU General Public License along with SCLS. If not, see <https://www.gnu.org/licenses/>.
//

// Include the good header file
#include "../headers/launcher.h"

// The namespace "as_launcher" is used to simplify the all.
namespace as_launcher {
    // Datas of the server
    std::string __server_ip = std::string();int __server_port;

    // Hub_Page constructor
    Hub_Page::Hub_Page(scls::_Window_Advanced_Struct* window_struct, std::string name):scls::GUI_Page(window_struct, name){
        if(std::filesystem::exists(".env")) {
            std::shared_ptr<scls::XML_Text> configuration_datas = scls::xml(window_struct->balises_shared_ptr(), scls::read_file(".env"));

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
    }

    // Loads an object in a page from XML
    std::shared_ptr<scls::GUI_Object> Hub_Page::__create_loaded_object_from_type(std::string object_name, std::string object_type, scls::GUI_Object* parent) {
        // Home page
        if(object_name == "hub_home_page") {a_home_page = *parent->new_object<scls::GUI_Object>(object_name);return a_home_page;}
        GUI_OBJECT_CREATION(scls::GUI_Text, a_home_software_button, "hub_shop")

        // Software page
        GUI_OBJECT_CREATION(scls::GUI_Object, a_software_page, "hub_software_page")
        GUI_OBJECT_CREATION(scls::GUI_Scroller_Choice, a_software_list, "hub_software_list")
        return scls::GUI_Page::__create_loaded_object_from_type(object_name, object_type, parent);
    }

    // Path to the datas of the software
    std::string Hub_Page::datas_path(){std::string current_path = scls::current_user_home_directory() + std::string("/AppData/Local/AsterSystem/");if(!std::filesystem::exists(current_path)){std::filesystem::create_directory(current_path);}current_path += std::string("Launcher/");if(!std::filesystem::exists(current_path)){std::filesystem::create_directory(current_path);}return current_path;}

    // Deletes the result of the installer
    void Hub_Page::installer_result_delete() {
        std::string needed_path = std::string("./installer/bin/Release/result.txt");
        if(!std::filesystem::exists(needed_path)){needed_path = std::string("./installer/result.txt");}
        if(std::filesystem::exists(needed_path)){std::filesystem::remove(std::string(needed_path));}
    }

    // Installs a software
    bool installation_launched = false;int installation_software_id = 0;
    void Hub_Page::install_software(int id) {
        installation_launched = true;installer_result_delete();installation_software_id = id;
        std::string software_name = std::string("\"software_\"") + std::to_string(id) + std::string(".zip\"");
        if(std::filesystem::exists(std::string("./installer/installer.exe"))){ShellExecute(NULL, "runas", "installer.exe", (std::string("\"0\" \"") + scls::program_directory() + std::string("Aster_System\\\\\" ") + software_name).c_str(), "./installer/", SW_SHOWNORMAL);}
        else{ShellExecute(NULL, "runas", "installer.exe", (std::string("\"0\" \"") + scls::program_directory() + std::string("Aster_System\\\\\" ") + software_name).c_str(), "./installer/bin/Release/", SW_SHOWNORMAL);}
    }

    // Returns a software in std::string
    std::string Hub_Page::Availabled_Software::software_to_std_string() const {
        std::string installed_string = std::string("0");if(installed){installed_string = std::string("1");}
        std::string to_return = std::string("<software id=\"") + std::to_string(id);
        to_return += std::string("\" name=\"") + name;
        to_return += std::string("\" installed=\"") + installed_string;
        to_return += std::string("\" installation_path=\"") + installation_path;
        to_return += std::string("\" execution_path=\"") + execution_path;
        to_return += std::string("\" major_version=\"") + std::to_string(major_version);
        to_return += std::string("\" minor_version=\"") + std::to_string(minor_version);
        to_return += std::string("\" newest_major_version=\"") + std::to_string(newest_major_version);
        to_return += std::string("\" newest_minor_version=\"") + std::to_string(newest_minor_version) + std::string("\">");
        return to_return;
    }

    // Launches a software
    void Hub_Page::launch_software(int id) {
        Availabled_Software* needed_software = software_by_id(id);
        ShellExecute(NULL, "open", (needed_software->installation_path + needed_software->execution_path).c_str(), NULL, needed_software->installation_path.c_str(), SW_SHOWNORMAL);
    }

    // Loads the softwares
    int __gen = 0;scls::Server_Response __network_response;std::thread __network_thread;
    bool __network_loaded = false;bool __network_validation = false;
    bool request_launched = false;
    void Hub_Page::load_softwares() {
        softwares().clear();
        __load_softwares_available_memory();
        if(!__network_loaded && !request_launched) {
            request_launched = true;
            int result = scls::send_request_asynchronous(__server_ip, __server_port, std::string("software"), &__network_response, &__network_validation, &__network_thread);
        }

        // Create the needed graphic elements
        software_list_availabled()->reset();software_list_installed()->reset();
        for(int i = 0;i<static_cast<int>(softwares().size());i++) {
            if(softwares().at(i).installed) {
                std::shared_ptr<scls::GUI_Scroller_Choice> section = *software_list_installed()->add_sub_section(std::string("software_list_child_") + std::to_string(__gen), softwares().at(i).name);
                section.get()->add_object(std::string("launch-") + std::to_string(softwares().at(i).id), std::string("Lancer"));
                section.get()->add_object(std::string("uninstall-") + std::to_string(softwares().at(i).id), std::string("Supprimer"));
            }
            else{
                std::shared_ptr<scls::GUI_Scroller_Choice> section = *software_list_availabled()->add_sub_section(std::string("software_list_child_") + std::to_string(__gen), softwares().at(i).name);
                section.get()->add_object(std::string("install-") + std::to_string(softwares().at(i).id), std::string("Installer"));
            }
            __gen++;
        }
        software_list_availabled()->show_objects();software_list_installed()->show_objects();
    }

    // Makes loading softwares faster
    void __load_software_object(Hub_Page::Availabled_Software& software, std::shared_ptr<scls::XML_Text> xml) {
        // Load the softwares
        std::vector<scls::XML_Attribute> attributes = xml.get()->xml_attributes();
        for(int i = 0;i<static_cast<int>(attributes.size());i++) {
            if(attributes.at(i).name == std::string("id")){software.id = std::stoi(attributes.at(i).value);}
            else if(attributes.at(i).name == std::string("execution_path")){software.execution_path = attributes.at(i).value;}
            else if(attributes.at(i).name == std::string("installation_path")){software.installation_path = attributes.at(i).value;}
            else if(attributes.at(i).name == std::string("installed")){software.installed = (attributes.at(i).value == std::string("1"));}
            else if(attributes.at(i).name == std::string("major_version")){software.major_version = std::stoi(attributes.at(i).value);}
            else if(attributes.at(i).name == std::string("minor_version")){software.minor_version = std::stoi(attributes.at(i).value);}
            else if(attributes.at(i).name == std::string("name")){software.name = attributes.at(i).value;}
            else if(attributes.at(i).name == std::string("newest_major_version")){software.newest_major_version = std::stoi(attributes.at(i).value);}
            else if(attributes.at(i).name == std::string("newest_minor_version")){software.newest_minor_version = std::stoi(attributes.at(i).value);}
        }

        // Check installation
        if(software.id != 0){software.installed = !software.installation_path.empty() && std::filesystem::exists(software.installation_path) && !software.execution_path.empty() && std::filesystem::exists(software.installation_path + software.execution_path);}
    }
    void __load_software_objects(std::vector<Hub_Page::Availabled_Software>& softwares, std::string content) {
        std::shared_ptr<scls::__Balise_Container> needed_balises = std::make_shared<scls::__Balise_Container>();
        std::shared_ptr<scls::XML_Text> xml = scls::xml(needed_balises, content);

        // Handle the softwares
        for(int i = 0;i<static_cast<int>(xml.get()->sub_texts().size());i++) {
            if(xml.get()->sub_texts().at(i).get()->xml_balise_name() == std::string("software")) {
                // Get the datas about the current software
                softwares.push_back(Hub_Page::Availabled_Software());
                __load_software_object(softwares[softwares.size() - 1], xml.get()->sub_texts().at(i));
            }
        }
    }
    // Loads the softwares in the memory
    void Hub_Page::__load_softwares(std::string network_text){
        std::vector<Hub_Page::Availabled_Software> current_softwares = softwares();
        std::vector<Hub_Page::Availabled_Software> network_response = std::vector<Hub_Page::Availabled_Software>();
        __load_software_objects(network_response, network_text);
        softwares().clear();

        // Handle the already-loaded datas
        for(int i = 0;i<static_cast<int>(current_softwares.size());i++) {
            for(int j = 0;j<static_cast<int>(network_response.size());j++) {
                if(current_softwares.at(i).id == network_response.at(j).id) {
                    // Create the software
                    Hub_Page::Availabled_Software to_add = current_softwares.at(i);

                    // Handle version
                    to_add.newest_major_version = network_response.at(j).major_version;
                    to_add.newest_minor_version = network_response.at(j).minor_version;

                    // Add the software
                    softwares().push_back(to_add);

                    // Finish the handling
                    network_response.erase(network_response.begin() + j);
                    current_softwares.erase(current_softwares.begin() + i);i--;break;
                }
            }
        }

        // Handle the newly loaded datas
        for(int i = 0;i<static_cast<int>(network_response.size());i++) {
            Hub_Page::Availabled_Software to_add = network_response.at(i);
            softwares().push_back(to_add);
        }

        // Finish the work
        __network_loaded = true;
        load_softwares();
        save_softwares();
        __network_loaded = false;
        request_launched = false;
    }
    void Hub_Page::__load_softwares_available_memory() {
        // Get the needed datas
        std::string memory_path = datas_path() + std::string("memory/");
        if(!std::filesystem::exists(memory_path)){std::filesystem::create_directory(memory_path);}
        memory_path += std::string("softwares.txt");
        std::string content = std::string();if(std::filesystem::exists(memory_path)){content = scls::read_file(memory_path);}

        // Get the balises
        __load_software_objects(softwares(), content);
    }
    // Saves the softwares in the memory
    void Hub_Page::save_softwares() {
        std::string content = std::string("");
        for(int i = 0;i<static_cast<int>(softwares().size());i++) {
            content += softwares().at(i).software_to_std_string();
        }

        // Write the datas
        std::string memory_path = datas_path() + std::string("memory/");
        if(!std::filesystem::exists(memory_path)){std::filesystem::create_directory(memory_path);}
        memory_path += std::string("softwares.txt");
        scls::write_in_file(memory_path, content);
    }
    // Uninstalls a software
    void Hub_Page::uninstall_software(int id) {
        installer_result_delete();
        Availabled_Software* needed_software = software_by_id(id);
        std::string software_name = std::string("\"software_\"") + std::to_string(id) + std::string(".zip\"");
        if(std::filesystem::exists(std::string("./installer/installer.exe"))){ShellExecute(NULL, "runas", "installer.exe", (std::string("\"1\" \"") + needed_software->installation_path + std::string("\\\" ") + software_name).c_str(), "./installer/", SW_SHOWNORMAL);}
        else{ShellExecute(NULL, "runas", "installer.exe", (std::string("\"1\" \"") + needed_software->installation_path + std::string("\\\" ") + software_name).c_str(), "./installer/bin/Release/", SW_SHOWNORMAL);}
        needed_software->execution_path = std::string("");needed_software->installation_path = std::string("");needed_software->installed = false;
        load_softwares();
    }

    // Hub_Page destructor
    Hub_Page::~Hub_Page(){if(request_launched && __network_thread.joinable()) {__network_thread.detach();}}

    //******************
    //
    // Check the events
    //
    //******************

    // Checks the events of navigation
    void Hub_Page::check_navigation(){
        if(home_software_button()->is_clicked_during_this_frame(GLFW_MOUSE_BUTTON_LEFT)){display_page(std::string("software"));}
    }

    // Checks the events of softwares page
    void Hub_Page::check_softwares_page() {
        if(__network_validation) {
            // Resets the datas
            if(__network_thread.joinable()){__network_thread.join();}__network_thread = std::thread();__network_validation=false;
            if(__network_response.datas.get() != 0 && __network_response.datas.get()->datas_size() > 0){__load_softwares(__network_response.datas.get()->extract_string(__network_response.datas.get()->datas_size()));}
        }

        if(window_struct()->key_pressed_during_this_frame("a") && !request_launched){load_softwares();}

        // Operate on a software
        if(software_list_availabled()->selection_modified()) {
            // Get the operation
            std::string operation = software_list_availabled()->currently_selected_objects_during_this_frame()[0].name();
            software_list_availabled()->unselect_object(software_list_availabled()->currently_selected_objects_during_this_frame()[0]);

            // Parse the operation
            std::vector<std::string> cutted = scls::cut_string(operation, std::string("-"));
            if(cutted.at(0) == std::string("install") && !installation_launched) {install_software(std::stoi(cutted.at(1)));}
        }

        // Operate on an installed software
        if(software_list_installed()->selection_modified()) {
            // Get the operation
            std::string operation = software_list_installed()->currently_selected_objects_during_this_frame()[0].name();
            software_list_installed()->unselect_object(software_list_installed()->currently_selected_objects_during_this_frame()[0]);

            // Parse the operation
            std::vector<std::string> cutted = scls::cut_string(operation, std::string("-"));
            if(cutted.at(0) == std::string("launch")) {launch_software(std::stoi(cutted.at(1)));}
            else if(cutted.at(0) == std::string("uninstall")) {uninstall_software(std::stoi(cutted.at(1)));}
        }
    }

    // Displays a page by its name
    void Hub_Page::display_page(std::string needed_page) {
        if(needed_page == std::string("home")){display_home_page();}
        else if(needed_page == std::string("software")){display_software_page();load_softwares();}

        // If the page is unknown
        else {scls::print("Aster System Launcher", std::string("Unknown page \"") + needed_page + std::string("\"."));return;}
    }

    // Updates the events
    void Hub_Page::update_event() {
        scls::GUI_Page::update_event();

        // Check the navigation
        check_navigation();

        // Check the softwares page
        if(current_page() == AS_SOFTWARE_PAGE){check_softwares_page();}

        // Check an installation
        if(installation_launched) {
            std::string needed_path = std::string("./installer/bin/Release/result.txt");
            if(!std::filesystem::exists(needed_path)){needed_path = std::string("./installer/result.txt");}
            if(std::filesystem::exists(needed_path)) {
                // Get the datas
                std::shared_ptr<scls::XML_Text> content = scls::xml(window_struct()->balises_shared_ptr(), scls::read_file(needed_path));
                scls::XML_Text* error = content.get()->balise_by_name("error");scls::XML_Text* software = content.get()->balise_by_name("software");
                if(software != 0) {
                    // Successfull installation
                    std::string execution_path = std::string();std::string installation_path = std::string();
                    int major_version = software_by_id(installation_software_id)->newest_major_version;
                    int minor_version = software_by_id(installation_software_id)->newest_minor_version;
                    for(int i = 0;i<static_cast<int>(software->xml_attributes().size());i++){
                        if(software->xml_attributes().at(i).name == std::string("execution_path")){execution_path = software->xml_attributes().at(i).value;}
                        else if(software->xml_attributes().at(i).name == std::string("installation_path")){installation_path = software->xml_attributes().at(i).value;}
                    }

                    // Handle the datas
                    software_by_id(installation_software_id)->execution_path = execution_path;
                    software_by_id(installation_software_id)->installation_path = installation_path;
                    software_by_id(installation_software_id)->installed = true;
                    software_by_id(installation_software_id)->major_version = major_version;
                    software_by_id(installation_software_id)->minor_version = minor_version;
                    save_softwares();
                    load_softwares();
                }
                else if(error != 0) {scls::print("Aster System Launcher", "Can't install the software.");}
                installer_result_delete();
                installation_launched = false;
            }
        }
    }
}
