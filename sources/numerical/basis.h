//
// $Source$
// $Date$
// $Revision$
//
// DESCRIPTION:
// Declaration of basis class for tableaus
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

#ifndef BASIS_H
#define BASIS_H

#include "base/base.h"
#include "math/gvector.h"
#include "math/gmatrix.h"

//---------------------------------------------------------------------------
// Class gbtBasis
//---------------------------------------------------------------------------

class gbtBasis {

private:
  gbtBlock<int> basis;        // current members of basis (neg for slacks)
  gbtBlock<int> cols;         // location of col in basis (0 if not in basis)
  gbtBlock<int> slacks;       // location of slacks in basis
  gbtArray<bool> colBlocked;  
  gbtArray<bool> rowBlocked;
  bool IsBasisIdent;

public:
  //-------------------------------------------
  // Constructors, Destructor, Operators
  //-------------------------------------------
  
  gbtBasis(int first, int last, int firstlabel, int lastlabel);
  gbtBasis(const gbtBasis &);
  virtual ~gbtBasis();
  
  gbtBasis& operator=(const gbtBasis&);

  //------------------------------
  // Public Members
  //------------------------------
  
  int First() const;         // First basis index
  int Last() const;          // Last  basis index
  int MinCol() const;    // First Column label
  int MaxCol() const;     // Last Column label

  bool IsRegColumn( int col ) const;
  bool IsSlackColumn( int col ) const; 

  //remove outindex, insert label, return outlabel
  int Pivot(int outindex, int col); 

  // return true iff label is a gbtBasis member
  bool Member(int label) const;

  // finds gbtBasis index corresponding to label number,
  int Find(int label) const;

  // finds label of variable corresponding to gbtBasis index
  int Label(int index) const;

  // marks/unmarks label to block it from entering basis
  void Mark(int label);
  void UnMark(int label);

  // returns true if label is blocked from entering basis
  bool IsBlocked(int label) const;

  // Check if gbtBasis is Ident
  virtual void CheckBasis();
  // returns whether the basis is the identity matrix
  bool IsIdent();

  void Dump(std::ostream &) const;
};

#endif // BASIS_H
