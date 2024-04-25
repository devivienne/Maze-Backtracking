#include"player.h"
#include"entity.h"
#include"stack.h"
#include"dllist.h"
#include"room.h"
#include"maze.h"
#include<iostream>


//  Player( ... )
//		Constructs and initializes the Player/Entity and its member variables
//		Remembers and discovers the starting room.
Player::Player(Maze* maze, Room p, std::string name, char sprite, const bool tbacktrack)
	:Entity(maze, p, name, sprite),
	m_lookingPaper(),
	m_discoveredRooms(),
	m_btQueue(),
	m_btStack(),
	BACKTRACKENABLED(tbacktrack) {

	// Discover the starting room
	m_discoveredRooms.insert_front(p);
	m_lookingPaper.enqueue(p);
}

//  stuck() 
//		See if the player is stuck in the maze (no solution)
bool Player::stuck() const {
	return Entity::state() == State::NOEXIT;
}

//  foundExit()
//		See if the player has found the exit
bool Player::foundExit() const {
	return Entity::state() == State::EXIT;
}

//  getTargetRoom()
//		Get the room the player wants to look around next.
//      If m_lookingPaper is empty then return an invalid room.
Room Player::getTargetRoom() const {
	if (m_lookingPaper.empty()) return Room(-1, -1);
	return m_lookingPaper.peek();
}

// discovered
//		returns true if the item is in the list
bool Player::discovered(const Room &p) const {
	return (m_discoveredRooms.find(p) != -1);
}



// say()
//		What does the player say?
//		Already Implemented, nothing to do here
void Player::say() {

	// Freedom supercedes being eaten
	if (state() == State::EXIT) {
		std::cout << name() << ": WEEEEEEEEE!";
		return;
	} 
 
	// Being eaten supercedes being lost
	switch (interact()) {
	case Interact::ATTACK:
		std::cout << name() << ": OUCH!";
		break;
	case Interact::GREET:
		break;
	case Interact::ALONE:
	default:
		switch (state()) {
		case State::LOOK:
			std::cout << name() << ": Where is the exit?";
			break;
		case State::NOEXIT:
			std::cout << name() << ": Oh no! I am Trapped!";
			break;
		case State::BACKTRACK:
			std::cout << name() << ": Got to backtrack...";
			break;
		default:
			break;
		}

		break;
	}
}


//  update() 
//		This function implements an algorithm to look through the maze
//      for places to move (STATE::LOOK). update also handles moving the 
//		player as well, if there is an open undiscovered cell to move to the  
//		player just moves there.  However, there will be cases in the next 
//		desired room to look around is not adjacent (more than one cell away), 
//		peek this room the player must backtrack (State::BACKTRACK) to a room 
//		that is adjacent to the next room the player was planning to visit
//
//		The player can only do one thing each call, they can either LOOK or 
//		BACKTRACK not both.  Nor should the player move more than one cell per
//		call to update.  If you examine the function calls for the entire game
//		you should observe that this is already be called within a loop. Your
//		Implementation should NOT have any loops that pertain to actual movement 
//		of the player.  
//
//		Backtracking is challenging, save it for the very very very last thing.
//		Make sure the STATE::LOOK aspect compiles and works first.
void Player::update() {
    if (m_lookingPaper.empty()) {
        state(State::NOEXIT);
        return;
    }
    if (BACKTRACKENABLED) {
        m_btQueue.enqueue(room());
        Room adjacent = getTargetRoom();
        if (!adjacent.adjacent(room())) {
            state(State::BACKTRACK);
            move(m_btStack.peek());
            m_btStack.pop();
            m_btList.insert_front(room());
            return;
        }
        while (!m_btList.empty()) {
            m_btStack.push(m_btList.peek_front());
            m_btList.remove_front();
        }
        while (!m_btQueue.empty()) {
            m_btStack.push(m_btQueue.peek());
            m_btQueue.dequeue();
        }
    }
    
    Room r = m_lookingPaper.peek();
    m_lookingPaper.dequeue();
    move(r);
    if (maze()->foundExit(r)) {
        state(State::EXIT);
        return;
    }

    Room west = Room(r.x() - 1, r.y());
    if (maze()->open(west) && !discovered(west)) {
        m_discoveredRooms.insert_front(west);
        m_lookingPaper.enqueue(west);
    }
    Room east = Room(r.x() + 1, r.y());
    if (maze()->open(east) && !discovered(east)) {
        m_discoveredRooms.insert_front(east);
        m_lookingPaper.enqueue(east);
    }
    Room north = Room(r.x(), r.y() - 1);
    if (maze()->open(north) && !discovered(north)) {
        m_discoveredRooms.insert_front(north);
        m_lookingPaper.enqueue(north);
    }
    Room south = Room(r.x(), r.y() + 1);
    if (maze()->open(south) && !discovered(south)) {
        m_discoveredRooms.insert_front(south);
        m_lookingPaper.enqueue(south);
    }

   
}
