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

#ifndef AS_LAUNCHER
#define AS_LAUNCHER

// Include the good header file
#include "../../scls-graphic-benoit/scls_graphic.h"
#include "../../scls_network_tim/scls_network.h"

// Define a macro to easily add an object
// Create the object
#define GUI_PAGE(type, gui, gui_page, function, display_function, display_function_parent) private: std::shared_ptr<type> gui; \
public: inline type* function() const {return gui.get();} \
public: void display_function(){if(function()==0){scls::print("Warning", std::string("SCLS GUI Page \"") + name() + std::string("\""), std::string("The object \"") + std::to_string(gui_page) + std::string("\" does not exists."));}else{display_function_parent();set_current_page(gui_page);function()->set_visible(true);}}
#define GUI_OBJECT(type, gui, function) private: std::shared_ptr<type> gui; \
public: inline type* function() const {return gui.get();}
// Get the object
#define GUI_OBJECT_CREATION(type, gui, name) else if(object_name == name){gui = *parent->new_object<type>(object_name);return gui;}
// Select the object
#define GUI_OBJECT_SELECTION(display_function, name) else if(page == name){display_function;}

// Define the existing pages
#define AS_HOME_PAGE 0
// Software page
#define AS_SOFTWARE_PAGE 100

// The namespace "as_launcher" is used to simplify the all.
namespace as_launcher {
    class Hub_Page : public scls::GUI_Page {
        // Class representating the "Hub" page for AS
    public:

        // Avaibled software
        struct Availabled_Software {
            // ID of the software
            int id = 0;
            // Execution path of the software
            std::string execution_path = std::string();
            // If the software is installed or not (and its path)
            std::string installation_path = std::string();
            bool installed = false;
            // Versions of the software
            int major_version = 0;int minor_version = 0;
            // Name of the software
            std::string name = std::string();

            // Returns a software in std::string
            std::string software_to_std_string() const;
        };

        // Hub_Page constructor
        Hub_Page(scls::_Window_Advanced_Struct* window_struct, std::string name);
        // Hub_Page destructor
        virtual ~Hub_Page();
        // Loads an object in a page from XML
        virtual std::shared_ptr<scls::GUI_Object> __create_loaded_object_from_type(std::string object_name, std::string object_type, scls::GUI_Object* parent);

        // Path to the datas of the software
        std::string datas_path();

        // Installs a software
        void install_software(int id);
        // Launches a software
        void launch_software(int id);
        // Loads the softwares
        void load_softwares();
        // Loads the softwares in the memory
        void __load_softwares(std::string network_text);
        void __load_softwares_available_memory();
        // Saves the softwares in the memory
        void save_softwares();

        // Returns / resets the current page
        inline unsigned short current_page() const {return a_current_state.current_page;};
        inline void set_current_page(unsigned short new_page) {a_current_state.current_page = new_page;};

        // Getters and setters
        inline Availabled_Software* software_by_id(int id){for(int i = 0;i<static_cast<int>(a_current_state.softwares.size());i++){if(a_current_state.softwares.at(i).id == id){return &a_current_state.softwares[i];}} return 0;};
        inline std::vector<Availabled_Software>& softwares(){return a_current_state.softwares;};

        //******************
        //
        // Check the events
        //
        //******************

        // Checks the events of hiding each pages
        void check_hiding(){};

        // Checks the events of navigation
        void check_navigation();
        // Checks the events of softwares page
        void check_softwares_page();
        // Updates the events
        virtual void update_event();

        //******************
        //
        // GUI handling
        //
        //******************

        // Home page
        GUI_PAGE(scls::GUI_Object, a_home_page, AS_HOME_PAGE, home_page, display_home_page, hide_all);
        GUI_OBJECT(scls::GUI_Text, a_home_software_button, home_software_button);

        // Software page
        GUI_PAGE(scls::GUI_Object, a_software_page, AS_SOFTWARE_PAGE, software_page, display_software_page, hide_all);
        GUI_OBJECT(scls::GUI_Scroller_Choice, a_software_list, software_list);
        inline scls::GUI_Scroller_Choice* software_list_availabled(){return software_list()->sub_section(std::string("availabled"));}
        inline scls::GUI_Scroller_Choice* software_list_installed(){return software_list()->sub_section(std::string("installed"));}

        // Hides all the pages
        void hide_all(){hide_sub_pages(true);check_hiding();};
        // Displays a page by its name
        void display_page(std::string needed_page);

   private:

        // Current state of the page
        struct {
            // Current page
            unsigned short current_page = AS_HOME_PAGE;

            // Loaded softwares
            std::vector<Availabled_Software> softwares;
        } a_current_state;

        //******************
        // GUI handling
        //******************
    };

    class AS_Window : public scls::Window {
        // Window in AS
    public :
        // AS_Window constructor
        AS_Window(unsigned int w, unsigned int h, std::string path) : scls::Window(w, h, path) { set_window_title(std::string("PLEOS - Hub")); }

        // Render the window
        virtual void render(){
            bool good = false;
            for(int i = 0;i<static_cast<int>(displayed_pages_2d().size());i++) {if(displayed_pages_2d().at(i).get()->should_render_during_this_frame()){good = true;break;}}
            if(good || displayed_pages_2d_modified_during_this_frame()){scls::Window::render();}
            else{scls::Window::render_always();}
        };

        // Create an object in the window
        std::shared_ptr<scls::Object> __create_loaded_object_from_type(std::string object_name, std::string object_type) {
            if(object_name == "hub") {std::shared_ptr<Hub_Page> hub = *new_page_2d<Hub_Page>(object_name);return hub;}
            return scls::Window::__create_loaded_object_from_type(object_name, object_type);
        }
    };
}

#endif // AS_LAUNCHER
