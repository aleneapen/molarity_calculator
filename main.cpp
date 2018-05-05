#include <FL/Fl.H>
#include <FL/Fl_Float_Input.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Input_Choice.H>
#include <FL/Fl_Button.H>
#include <string>
#include <vector>
#include <cstring>
#include <stdlib.h>
#include <map>
#include <bitset>
#include <FL/fl_ask.H>

// For window icon on windows
#ifdef __MINGW32__
#include <FL/x.H> // For windows icon
#include <windows.h>
#endif

#define COLS 4
#define ROWS 5
#define WIDTH 500
#define HEIGHT 200


// Constants: row headers
const static char* row_header[ROWS] = {
            "Mass","Molar mass","Moles","Volume", "Molarity"
        };

// Constants: row_header in enum, each header should convert to binary with "on" for corresponding header bit (read right to left). THE ORDER OF ITEMS IN row_header AND RowEnum SHOULD MATCH, ELSE THE CALCULTATIONS WILL BE MESSED UP
enum RowEnum {
    mass = 1,
    molar_mass = 2,
    moles = 4,
    volume = 8,
    molarity = 16,
    all = 31,
    none = 0
};

// Constants: units, represented as a vector of maps. Each map in the vector corresponds to units for each measures from row_header. The map key is the C string for units used as labels, and value is the factor used by the get_value and set_value functions of the Calculator class.
const static std::vector<std::map<const char*, double>> units_vector
{
    { // Mass map

        {
            "milligrams", 0.001
        },
        {
            "micrograms", 0.000001
        },
        {
            "nanograms", 0.000000001
        },
        {
            "grams", 1
        },
        {
            "kilograms", 1000
        }

    },
    { // molar_mass map

        {
            "/g/mol", 1
        },
        {
            "/mg/mol", 0.001
        },
        {
            "/g/mmol", 1000
        }

    },
    { // moles map

        {
            "mol", 1
        },
        {
            "mmol", 0.001
        },
        {
            "umol", 0.000001
        }

    },
    { // Volume map

        {
            "mL", 0.001
        },
        {
            "uL", 0.000001
        },
        {
            "nL", 0.000000001
        },
        {
            "L", 1
        }


    },
    { // Concentration map
        {
            "mM", 1e-3
        },
        {
            "uM", 1e-6
        },
        {
            "nM", 1e-9
        },
        {
            "pM", 1e-12
        },
        {
            "M", 1
        },
    }
};



// Constants: Colour enum
enum Colour {
    red = FL_RED,
    green = FL_DARK_GREEN,
    black = FL_BLACK,
    blue = FL_BLUE
};

// Constants: Fonts
enum FontType {
    bold = FL_BOLD,
    normal = FL_HELVETICA
    
};

// Constants: Help messages




// Declarations of fltk callbacks
void calculate_cb(Fl_Widget*, long int);
void clear_cb(Fl_Widget*, void*);
void help_cb(Fl_Widget*);


// The Calculator container
class Calculator: public Fl_Group {
//    void* w[ROWS][COLS];        // widget pointers
    Fl_Box* box_ptrs[ROWS];
    Fl_Input_Choice* input_choice_ptrs[ROWS];
    Fl_Float_Input* float_input_ptrs[ROWS];
    Fl_Button* calc_button_ptrs[ROWS];
    
    Fl_Button* clear_button;
    Fl_Button* help_button;
    
public:

    // The get_value function returns the value from number input field at row p as a double.
    double get_value (long p) const
    {
        const char* value = (float_input_ptrs[p])->value();

        double d_value = atof(value);
        auto unit = (input_choice_ptrs[p])->value();
        const std::map<const char*,double>* unit_map = &(units_vector[p]);
        for (auto& elem: *unit_map)
            if (strcmp(elem.first,unit) == 0)
                return(elem.second*d_value);
    }
    
    
    // The set_value function sets the value of number field at row p.
    void set_value(long p, double value,bool empty = 0)
    {
        if (empty)
        {
            (float_input_ptrs[p])->value("");
            return;
        }
            
            
        auto unit = (input_choice_ptrs[p])->value();
        const std::map<const char*,double>* unit_map = &(units_vector[p]);
        for (auto& elem: *unit_map)
            if (strcmp(elem.first,unit) == 0)
                value/=elem.second;
        
        (float_input_ptrs[p])->value(std::to_string(value).c_str());
    }
    
    
    void set_colour(unsigned long bin, Colour c, FontType ft )
    {
        if (!bin)
            return;
        Fl_Widget* text_widget = nullptr;
        std::string bin_str = std::bitset<5>(bin).to_string();

        
        unsigned j = 0; // To iterate through rows
        for (auto p = bin_str.crbegin(); p!=bin_str.crend();++p)
        {
            text_widget = (Fl_Widget*)(box_ptrs[j++]);
            if (*p == '1')
            {
                
                text_widget->labelcolor(c);
                text_widget->labelfont(ft);
                
            }
            text_widget->redraw_label();
            
            
        }

    }
    
    
    // Clears all number input fields.
    void clear_inputs()
    {
        for (int r = 0; r!= ROWS; ++r)
            (float_input_ptrs[r])->value("");
    }
    
    // Constructor
    Calculator(int X, int Y, int W, int H, const char*L=0) : Fl_Group(X,Y,W,H,L) {  
        int cellw = 100;
        int cellh = 25;
        int xx = X, yy = Y;
        
        // Create widgets
        for ( int r=0; r<ROWS; r++ ) {
            for ( int c=0; c<COLS; c++ ) {
                if ( c==0 ) {
                    // c == 0 is the row header column
                    Fl_Box* box = new Fl_Box(xx,yy,cellw,cellh,row_header[r]);
                    box->box(FL_FLAT_BOX);
                    box->align(FL_ALIGN_INSIDE|FL_ALIGN_RIGHT);
                    this->box_ptrs[r] = box;
                } else if ( c==1 ) {
                    // c == 1 is the number input column
                    
                    Fl_Float_Input* in = new Fl_Float_Input(xx,yy,cellw+20,cellh); // Extra width for number input field
                    in->box(FL_BORDER_BOX);
                    this->float_input_ptrs[r] = in;
                    xx+=20; // Compensate for extra width
                } else if (c==2) {
                    // c == 2 is column for units
                    Fl_Input_Choice* choice = new Fl_Input_Choice(xx,yy,cellw,cellh);
                    
                    std::vector<const char*> choice_labels;
                    std::map<const char*,double> units_map = units_vector[r];
                    for (std::map<const char*,double>::iterator it = units_map.begin(); 
                            it!= units_map.end();++it)
                        choice_labels.push_back(it->first);
                    for (auto& i: choice_labels)
                        choice->add(i);
                    choice->value(choice_labels[0]);
                    choice->input()->readonly(1);
                    this->input_choice_ptrs[r] = choice;
                } else if (c==3) {
                    // c == 2 is column for calculation buttons
                    Fl_Button* calc_button = new Fl_Button(xx,yy,cellw,cellh,"Calculate");
                    calc_button->callback(calculate_cb,r);                    
                    this->calc_button_ptrs[r] = calc_button;
                    
                }
                
                xx += cellw;
            }
            xx = X;
            yy += cellh;
            
        }
        yy+=10;
       
        // The clear button
        Fl_Button *clear_button = new Fl_Button((WIDTH/4),yy,(WIDTH/4),cellh,"Clear (Ctrl+D)");
        clear_button->callback(clear_cb);
        this->clear_button = clear_button;
        
        
        // Help dialog
        Fl_Button *help_button = new Fl_Button((WIDTH/2),yy,(WIDTH/4),cellh,"Help");
        help_button->callback(help_cb);
        this->help_button = help_button;

        end();
    }
    
    
    // Destructor
    ~Calculator() 
    {
        for (unsigned i = 0; i < ROWS; ++i)
        {
            delete box_ptrs[i];
            delete calc_button_ptrs[i];
            delete float_input_ptrs[i];
            delete input_choice_ptrs[i];
            
        }
        delete help_button;
        delete clear_button;
    }
    

    // Handling keyboard shortcuts
    int handle(int e)
    {
        
        int ret = Fl_Group::handle(e);
        switch (e){            
            case FL_KEYDOWN:
            {
                
                if (Fl::event_key() == (FL_Enter) && (Fl::event_state(FL_CTRL)))
                {
                    auto current_widget_ptr = Fl::focus();
                    for (int t = 0; t< ROWS; ++t)
                    {
                        if (float_input_ptrs[t] == current_widget_ptr)
                        {
                            (calc_button_ptrs[t])->do_callback();
                        }
                            
                        ret = 1;
                    }
                }
                else if (Fl::event_key() == (FL_Enter))
                {
                    auto current_widget_ptr = Fl::focus();
                    for (int t = 0; t< ROWS; ++t)
                    {
                        if (float_input_ptrs[t] == current_widget_ptr)
                        {
                            if (ROWS)
                            {
                                Fl_Float_Input* input =  (t+1 == ROWS)? float_input_ptrs[0] : float_input_ptrs[t+1];
                                input->take_focus();
                            }
                        }
                            
                        ret = 1;
                    }
                }
                
                if (Fl::event_key() == 'd' && (Fl::event_state(FL_CTRL)))
                {
                    clear_button->do_callback();
                    ret = 1;
                    
                }
                break;
            }
                    
        }
        
        return (ret);
//        return(1);
    }
};



int main()
{
    
    
    Fl_Double_Window win(WIDTH,HEIGHT,"Molarity Calculator");
    Calculator calc(10,10,WIDTH-20,HEIGHT-20);
    
    #ifdef __MINGW32__
    win.icon((char*)LoadIcon(fl_display,MAKEINTRESOURCE(101)));
    #endif
    win.show();
    
    return (Fl::run());
}

// Definition of callbacks
void clear_cb(Fl_Widget* w, void* p) 
{
    Fl_Button* button = (Fl_Button*) w;
    Calculator* parent_calculator = (Calculator*)(button->parent());
    parent_calculator->clear_inputs();
    parent_calculator->set_colour(RowEnum::all,Colour::black,FontType::normal);
}

void help_cb(Fl_Widget* w) 
{
    fl_message_title("Help");
    fl_message("> Click calculate on each row to see required fields in red.\n"
        "> Fields used for calculation are shown in green.\n"
        "> Calculated field is shown in blue.\n"
        "> Use return key to cycle between input fields.\n"
        "> Ctrl+return to calculate current field.");
}


void calculate_cb(Fl_Widget* w, long int p)
{
    
    Fl_Button* button = (Fl_Button*) w;
    Calculator* parent_calculator = (Calculator*)(button->parent());

    double mass = parent_calculator->get_value(0),
    molar_mass = parent_calculator->get_value(1),
    moles = parent_calculator->get_value(2),
    volume = parent_calculator->get_value(3),
    molarity = parent_calculator->get_value(4),
    current = parent_calculator->get_value(p);
    
    
    
//    parent_calculator->set_colour(p,Colour::green);
    
    // First check if requested field has value
    

    
    
    
    parent_calculator->set_colour(RowEnum::all,Colour::black,FontType::normal);
    unsigned long good_font_condition = 0;
    unsigned long bad_font_condition = 0;
    
    mass ? (good_font_condition |= RowEnum::mass) : (bad_font_condition |= RowEnum::mass);
    molar_mass ? (good_font_condition |= RowEnum::molar_mass) : (bad_font_condition |= RowEnum::molar_mass);
    moles ? (good_font_condition |= RowEnum::moles) : (bad_font_condition |= RowEnum::moles);
    volume ? (good_font_condition |= RowEnum::volume) : (bad_font_condition |= RowEnum::volume);
    molarity ? (good_font_condition |= RowEnum::molarity) : (bad_font_condition |= RowEnum::molarity);
    
    
    // Calculate RowEnum value from p
    unsigned current_enum = 1;
    for (unsigned i = 1,j = 0; j!=p; i=current_enum, ++j)
        current_enum = i + i;
    

    if (current)
    {
        parent_calculator->set_value(p, 0, 1); // Set to empty
        good_font_condition ^= current_enum;
//        parent_calculator->set_colour(current_enum,Colour::black,FontType::normal); // Set to normal
    }
    
    // Calculations
    switch (p)
    {
        // Calculate mass from other values
        case 0:
            
            if (molar_mass)
            {
                
                if (volume && molarity)
                {
                    parent_calculator->set_value(p,volume*molarity*molar_mass);
                    parent_calculator->set_value(2,volume*molarity);
                    good_font_condition &= RowEnum::molar_mass | RowEnum::volume | RowEnum::molarity;
                    bad_font_condition = 0;
                }
                    
                else if (moles)
                {
                    parent_calculator->set_value(p,moles*molar_mass);
                    good_font_condition &= RowEnum::molar_mass | RowEnum::moles;
                }
                
            }
            else 
            {
                bad_font_condition &= RowEnum::molar_mass | RowEnum::volume | RowEnum::molarity ;
            }
            break;
            
        // Calculate molar mass
        case 1:
            if (mass)
            {
                if (volume && molarity)
                {
                    parent_calculator->set_value(p,mass/(volume*molarity));
                    parent_calculator->set_value(2,volume*molarity);
                    good_font_condition &= RowEnum::mass | RowEnum::volume | RowEnum::molarity;
                    bad_font_condition = 0;
                }
                else if (moles)
                {
                    parent_calculator->set_value(p,mass/moles);
                    good_font_condition &= RowEnum::moles | RowEnum::mass;
                }
            }
            else 
            {
                bad_font_condition &= RowEnum::mass | RowEnum::volume | RowEnum::molarity;
            }
            break;
        
        // Calculate Moles
        case 2:
            if (mass && molar_mass)
            {
                parent_calculator->set_value(p,mass/molar_mass);
                good_font_condition &= RowEnum::molar_mass | RowEnum::mass;
                
                // Set other 2 to empty
                parent_calculator->set_value(3,0,1);
                parent_calculator->set_value(4,0,1);
                
            }
            else if (volume && molarity)
            {
                parent_calculator->set_value(p,volume*molarity);
                good_font_condition &= RowEnum::molarity | RowEnum::volume;
            }
            
            break;
            
            
        // Calculate Volume
        case 3:
            if (molarity) 
            {
                if (mass && molar_mass)
                {
                    parent_calculator->set_value(p,(mass/molar_mass)/molarity);
                    parent_calculator->set_value(2,mass/molar_mass);
                    good_font_condition &= RowEnum::molarity | RowEnum::mass | RowEnum::molar_mass;
                    bad_font_condition = 0;
                }
                else if (moles)
                {
                    parent_calculator->set_value(p,moles/molarity);
                    good_font_condition &= RowEnum::molarity | RowEnum::moles;
                }
                               
            }
            else 
            {
                bad_font_condition &= RowEnum::molarity | RowEnum::mass | RowEnum::molar_mass;
            }
            break;
            
            
        // Calculate molarity
        case 4:
            if (volume) 
            {
                if (mass && molar_mass)
                {
                    parent_calculator->set_value(p,(mass/molar_mass)/volume);
                    parent_calculator->set_value(2,mass/molar_mass);
                    good_font_condition &= RowEnum::volume | RowEnum::mass | RowEnum::molar_mass;
                    bad_font_condition = 0;
                }
                else if (moles)
                {
                    parent_calculator->set_value(p,moles/volume);
                    good_font_condition &= RowEnum::volume | RowEnum::mass;
                }
            }
            else 
            {
                bad_font_condition &= RowEnum::volume | RowEnum::mass | RowEnum::molar_mass;
            }
            break;
            
        // Reset colours
        default:
            good_font_condition = 0;
            bad_font_condition = 0;
    }
    // Set colours
    parent_calculator->set_colour(good_font_condition,Colour::green,FontType::bold);
    parent_calculator->set_colour(bad_font_condition,Colour::red,FontType::bold);
    parent_calculator->set_colour(current_enum,Colour::blue,FontType::bold);
}
