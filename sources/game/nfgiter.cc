//
// $Source$
// $Date$
// $Revision$
//
// DESCRIPTION:
// Implementation of normal form iterators
//
// This file is part of Gambit
// Copyright (c) 2002, The Gambit Project
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//

#include "game.h"
#include "nfgiter.h"
#include "nfgciter.h"

//--------------------------------------------------------------------------
// gbtNfgIterator:  Constructors, Destructors, Operators
//--------------------------------------------------------------------------

gbtNfgIterator::gbtNfgIterator(gbtGame p_nfg)
  : m_nfg(p_nfg), current_strat(p_nfg->NumPlayers()), 
    profile(p_nfg->NewContingency())
{
  First();
}

gbtNfgIterator::gbtNfgIterator(const gbtNfgIterator &it)
  : m_nfg(it.m_nfg), current_strat(it.current_strat), 
    profile(it.profile)
{ }

gbtNfgIterator::gbtNfgIterator(const gbtNfgContIterator &p_iterator)
  : m_nfg(p_iterator.m_nfg), 
    current_strat(p_iterator.m_current), profile(p_iterator.m_profile)
{ }

gbtNfgIterator::~gbtNfgIterator()
{ }

gbtNfgIterator &gbtNfgIterator::operator=(const gbtNfgIterator &it)
{
  if (this != &it)  {
    m_nfg = it.m_nfg;
    profile = it.profile;
    current_strat = it.current_strat;
  }
  return *this;
}

//-----------------------------
// gbtNfgIterator: Member Functions
//-----------------------------

void gbtNfgIterator::First(void)
{
  for (int i = 1; i <= m_nfg->NumPlayers(); i++)  {
    gbtGameStrategy s = m_nfg->GetPlayer(i)->GetStrategy(1);
    profile->SetStrategy(s);
    current_strat[i] = 1;
  }
}

int gbtNfgIterator::Next(int p)
{
  gbtGamePlayer player = m_nfg->GetPlayer(p);
  if (current_strat[p] < player->NumStrategies()) {
    gbtGameStrategy s = player->GetStrategy(++(current_strat[p]));
    profile->SetStrategy(s);
    return 1;
  }
  else {
    gbtGameStrategy s = player->GetStrategy(1);
    profile->SetStrategy(s);
    current_strat[p] = 1;
    return 0;
  }
}

int gbtNfgIterator::Set(int p, int s)
{
  if (p <= 0 || p > m_nfg->NumPlayers() ||
      s <= 0 || s > m_nfg->GetPlayer(p)->NumStrategies()) {
    return 0;
  }
  
  profile->SetStrategy(m_nfg->GetPlayer(p)->GetStrategy(s));
  return 1;
}

void gbtNfgIterator::Get(gbtArray<int> &t) const
{
  for (int i = 1; i <= m_nfg->NumPlayers(); i++) {
    t[i] = profile->GetStrategy(m_nfg->GetPlayer(i))->GetId();
  }
}

void gbtNfgIterator::Set(const gbtArray<int> &t)
{
  for (int i = 1; i <= m_nfg->NumPlayers(); i++){
    profile->SetStrategy(m_nfg->GetPlayer(i)->GetStrategy(t[i]));
    current_strat[i] = t[i];
  } 
}

gbtGameOutcome gbtNfgIterator::GetOutcome(void) const
{
  return profile->GetOutcome();
}

void gbtNfgIterator::SetOutcome(gbtGameOutcome outcome)
{
  profile->SetOutcome(outcome);
}


//-------------------------------------
// gbtNfgContIterator: Constructor, Destructor
//-------------------------------------

gbtNfgContIterator::gbtNfgContIterator(const gbtGame &p_game)
  : m_current(p_game->NumPlayers()),
    m_nfg(p_game), m_profile(m_nfg->NewContingency()), 
    m_thawed(m_nfg->NumPlayers())
{
  for (int i = 1; i <= m_thawed.Length(); i++) {
    m_thawed[i] = i;
  }

  First();
}

gbtNfgContIterator::~gbtNfgContIterator()
{ }

//------------------------------
// gbtNfgContIterator: Member Functions
//------------------------------

void gbtNfgContIterator::First(void)
{
  for (int i = 1; i <= m_thawed.Length(); i++) {
    m_profile->SetStrategy(m_nfg->GetPlayer(m_thawed[i])->GetStrategy(1));
    m_current[m_thawed[i]] = 1;
  }	
}

void gbtNfgContIterator::Freeze(gbtGameStrategy p_strategy)
{
  int player = p_strategy->GetPlayer()->GetId();
  if (!m_frozen.Contains(player)) {
    m_frozen.Append(player);
    m_thawed.Remove(m_thawed.Find(player));
  }

  m_profile->SetStrategy(p_strategy);
  m_current[player] = p_strategy->GetId();
  First();
}

void gbtNfgContIterator::Thaw(gbtGamePlayer p_player)
{
  int pl = p_player->GetId();
  if (m_thawed.Contains(pl)) {
    return;
  }
  m_frozen.Remove(m_frozen.Find(pl));
  int i = 1;
  while (m_thawed[i] < pl)   i++;
  m_thawed.Insert(pl, i);
  First();
}

int gbtNfgContIterator::Next(gbtGamePlayer p_player)
{
  int p = p_player->GetId();
  if (!m_thawed.Contains(p)) {
    return 0;
  }

  if (m_current[p] < p_player->NumStrategies())  {
    gbtGameStrategy s = p_player->GetStrategy(++(m_current[p]));
    m_profile->SetStrategy(s);
    First();
    return 1;
  }
  else {
    gbtGameStrategy s = p_player->GetStrategy(1);
    m_profile->SetStrategy(s);
    m_current[p] = 1;
    First();
    return 0;
  }
}

int gbtNfgContIterator::NextContingency(void)
{
  int j = m_thawed.Length();
  if (j == 0) return 0;    	

  while (1)   {
    int pl = m_thawed[j];
    if (m_current[pl] < m_nfg->GetPlayer(pl)->NumStrategies()) {
      gbtGameStrategy s = m_nfg->GetPlayer(pl)->GetStrategy(++(m_current[pl]));
      m_profile->SetStrategy(s);
      return 1;
    }
    m_profile->SetStrategy(m_nfg->GetPlayer(pl)->GetStrategy(1));
    m_current[pl] = 1;
    j--;
    if (j == 0) {
      return 0;
    }
  }
}
