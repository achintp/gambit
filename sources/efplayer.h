//
// FILE: player.h -- Declaration of Player data type
//
// $Id$
//

#ifndef PLAYER_H
#define PLAYER_H

#include "gstring.h"
#include "gset.h"

#include "infoset.h"

class Player   {
  private:
    gString name;
    gSet<Infoset *> infosets;

  public:
	// CONSTRUCTORS AND DESTRUCTOR
	// initialize a player to defaults
    Player(void) : name("UNNAMED")   { }
	// initialize a player, with a specific name
    Player(const gString &s) : name(s)   { }
	// copy constructor
    Player(const Player &);
	// clean up after a player
    ~Player();

	// OPERATOR OVERLOADING
	// assignment operator
    Player &operator=(const Player &);

	// OPERATIONS ON INFOSETS
	// create a new infoset at which the player has the decision
	//  returns the number of the newly-created infoset
    int CreateInfoset(int branches)
      { return infosets.Append(new Infoset(branches)); }

	// remove an infoset
    void RemoveInfoset(int iset)
      { delete infosets.Remove(iset); }

	// returns the name of an infoset
    gString GetInfosetName(int iset) const
      { return infosets[iset]->GetInfosetName(); }

	// set the name of an infoset
    void SetInfosetName(int iset, const gString &name)
      { infosets[iset]->SetInfosetName(name); }

	// return the number of infosets at which player has the decision
    int NumInfosets(void) const
      { return infosets.Length(); }

	// OPERATIONS ON BRANCHES
	// returns the number of branches in an infoset
    int NumBranches(int iset) const
      { return infosets[iset]->NumBranches(); }

	// set the name of a branch in an infoset
    void SetBranchName(int iset, int br, const gString &name)
      { infosets[iset]->SetBranchName(br, name); }

	// returns the name of a branch in an infoset
    gString GetBranchName(int iset, int br) const
      { return infosets[iset]->GetBranchName(br); }

	// remove a branch from an infoset
    void RemoveBranch(int iset, int br)
      { infosets[iset]->RemoveBranch(br); }

	// insert a branch in an infoset
    void InsertBranch(int iset, int br)
      { infosets[iset]->InsertBranch(br); }


	// NAMING OPERATIONS
	// set the player's name
    void SetPlayerName(const gString &s)   { name = s; }

	// returns the player's name
    gString GetPlayerName(void) const    { return name; }


	// FILE OPERATIONS
    void WriteToFile(FILE *f, int plno) const;
};


// The PlayerSet is a gSet with the special feature that there are
// by default two players defined:  -1 (dummy) and 0 (chance)
// Players are thus (for external purposes) sequentially numbered from
// -1 through the highest "real" player number.

class PlayerSet   {
  private:
    gSet<Player *> players;

  public:
	// CONSTRUCTORS AND DESTRUCTOR
	// initialize the default PlayerSet
    PlayerSet(void);
	// copy constructor
    PlayerSet(const PlayerSet &);
	// clean up after a PlayerSet
    ~PlayerSet();

	// OPERATOR OVERLOADING
	// assignment operator
    PlayerSet &operator=(const PlayerSet &);

	// OPERATIONS ON PLAYERS
	// add player number p to the set
    void AddPlayer(int p);

	// returns the number of players in the set (not including
	//   dummy or chance)
    int NumPlayers(void) const
      { return (players.Length() - 2); }

	// set the name of a player
    void SetPlayerName(int p, const gString &name)
      { players[p + 2]->SetPlayerName(name); }

	// returns the name of a player
    gString GetPlayerName(int p) const
      { return players[p + 2]->GetPlayerName(); }

    	// OPERATIONS ON INFOSETS
	// create a new infoset for player p
	//  returns the number of the new infoset
    int CreateInfoset(int p, int branches) 
      { return players[p + 2]->CreateInfoset(branches); }

	// remove an infoset for player p
    void RemoveInfoset(int p, int iset)
      { players[p + 2]->RemoveInfoset(iset); }

	// returns the name of an infoset
    gString GetInfosetName(int p, int iset) const
      { return players[p + 2]->GetInfosetName(iset); }

	// set the name of an infoset
    void SetInfosetName(int p, int iset, const gString &s)
      { players[p + 2]->SetInfosetName(iset, s); }

	// returns the total number of infosets
    int NumInfosets(void) const;

	// returns the number of infosets for a player
    int NumInfosets(int p) const
      { return players[p + 2]->NumInfosets(); }

	// OPERATIONS ON BRANCHES
	// returns the number of branches in an infoset
    int NumBranches(int p, int iset) const
      { return players[p + 2]->NumBranches(iset); }

	// set the name of a branch
    void SetBranchName(int p, int iset, int br, const gString &name)
      { players[p + 2]->SetBranchName(iset, br, name); }

	// returns the name of a branch
    gString GetBranchName(int p, int iset, int br) const
      { return players[p + 2]->GetBranchName(iset, br); }

	// remove a branch from an infoset
    void RemoveBranch(int p, int iset, int br)
      { players[p + 2]->RemoveBranch(iset, br); }

	// insert a branch in an infoset
    void InsertBranch(int p, int iset, int br)
      { players[p + 2]->InsertBranch(iset, br); }

	// FILE OPERATIONS
    void WriteToFile(FILE *f) const;
    
};
#endif    // PLAYER_H


