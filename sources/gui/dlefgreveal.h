//
// FILE: dlefgreveal.h -- Selection of players for Edit->Infoset->Reveal
//
// $Id$
//

#ifndef DLEFGREVEAL_H
#define DLEFGREVEAL_H

class dialogInfosetReveal : public guiAutoDialog {
private:
  const Efg::Game &m_efg;

  wxListBox *m_playerNameList;

  const char *HelpString(void) const { return "Edit Menu (efg)"; }

public:
  dialogInfosetReveal(const Efg::Game &, wxWindow *);
  virtual ~dialogInfosetReveal() { } 

  gArray<EFPlayer *> GetPlayers(void) const;
};

#endif  // DLEFGREVEAL_H