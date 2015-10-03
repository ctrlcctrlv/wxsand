#ifndef ELEMENTH
#define ELEMENTH

#include <wx/wx.h>

#define MAX_TEMP 100
#define MAX_PROB 0.99

class Reaction {
 public:
  Reaction(wxString filename);
  ~Reaction();

  wxString m_reactant1;
  wxString m_reactant2;

  wxString m_product1;
  wxString m_product2;

  double m_probability_at_zero;
  double m_probability_at_limit;
  
  int m_change_in_energy;
};


class Element {
 public:
  Element(wxString filename);
  ~Element();

  wxString m_name;
  wxColor m_coldcolor;
  wxColor m_hotcolor;
  double m_gravity;
  double m_slip;
  double m_density;
  double m_conductivity;
  bool m_visible;
  wxString m_description;

};

void FillGlobalProbabilityArrays(void);


#endif
