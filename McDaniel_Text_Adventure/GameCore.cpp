#include "GameCore.h"
#include "CommandParser.h"
#include <stdio.h>

RoomData CreateRoom(const std::string& inName, const std::string& inDescription, bool hasKey, bool hasLadder)
{
	RoomData room = {};
	room.Name = inName;
	room.Description = inDescription;
	room.HasKey = hasKey;
	room.HasLadder = hasLadder;

	return room;
}

void AddExitToRoom(RoomData& roomToEdit, const std::string& exitName, int targetRoomIndex, bool isLocked)
{
	RoomExitData exit = {};
	exit.Name = exitName;
	exit.TargetRoomIndex = targetRoomIndex;
	exit.Locked = isLocked;
	roomToEdit.Exits.push_back(exit);
}

void InitializeGame(PlayerState& playerState, WorldState& worldState)
{
	printf("You've come home from a long day of work. \nAfter exiting your car you realize that you don't have your house keys on you.\n");

	playerState.CurrentRoomIndex = 0;
	playerState.WantsToLook = true;
	playerState.HasKey = false;
	playerState.HasLadder = false;

	RoomData room1 = CreateRoom("garage", "Your cluttered one car garage filled with various useful items that you barely ever use.", false, true);
	AddExitToRoom(room1, "porch", 1, false);
	AddExitToRoom(room1, "backyard", 2, false);
	AddExitToRoom(room1, "car", 5, false);
	worldState.Rooms.push_back(room1);

	RoomData room2 = CreateRoom("porch", "Just an average porch with creaky floor boards that you meant to repair 3 months ago.", false, false);
	AddExitToRoom(room2, "garage", 0, false);
	AddExitToRoom(room2, "backyard", 2, false);
	AddExitToRoom(room2, "house", 6, true);
	worldState.Rooms.push_back(room2);

	RoomData room3 = CreateRoom("backyard", "Standing here gives you a swampy, miserable view of the back of your house.\nThere's a window open on the second story.", false, false);
	AddExitToRoom(room3, "garage", 0, false);
	AddExitToRoom(room3, "porch", 1, false);
	AddExitToRoom(room3, "window", 3, false);
	worldState.Rooms.push_back(room3);

	RoomData room4 = CreateRoom("window","You lean the ladder against your house and start to climb.\nYou struggle to climb through the half open window, suddenly you hear police sirens.\nYour nosy neighbor called the cops on a suspicious individual breaking into your home.\nYou try to explain to the police that this is all a big misunderstanding but they put you in cuffs and throw you in the back of the police car.\nAt least you don't have to go to work tomorrow.\nGAME END!\nQuit the game...", false, false);
	AddExitToRoom(room4, "porch", 1, false);
	worldState.Rooms.push_back(room4);

	RoomData room5 = CreateRoom("work", "It's like crawling back into Hell.", true, false);
	AddExitToRoom(room5, "car", 5, false);
	worldState.Rooms.push_back(room5);

	RoomData room6 = CreateRoom("car", "Your check engine light is on, but you choose to ignore it.", false, false);
	AddExitToRoom(room6, "garage", 0, false);
	AddExitToRoom(room6, "work", 4, false);
	worldState.Rooms.push_back(room6);

	RoomData room7 = CreateRoom("house", "Home sweet home. \nYou can finally rest after a long day of work. \nRest up for another day of labor in the wagey cagey.\n\nGAME END! \nQuit the game...", false, false);
	worldState.Rooms.push_back(room7);

}

void GetInput(PlayerState& playerState, const WorldState& worldState)
{
	playerState.WantsToLook = false;
	playerState.DesiredExit = "";
	playerState.DesiredPickup = "";

	printf("What do you do?\n");
	printf("> ");
	TextAdventureCommand command = ParseAdventureCommand();
	if (command.Verb == "quit")
	{
		playerState.WantsToExit = true;
	}
	else if (command.Verb == "look")
	{
		playerState.WantsToLook = true;
	}
	else if (command.Verb == "go")
	{
		playerState.DesiredExit = command.Parameter;
	}
	else if (command.Verb == "get")
	{
		playerState.DesiredPickup = command.Parameter;
	}
	else if (command.Verb == "help")
	{
		printf("Command List: look, quit, go [place], get [object]\n");
	}
	else
	{
		printf("What were you doing again?\n");
	}
	printf("\n");

}

void RenderGame(const PlayerState& playerState, const WorldState& worldState)
{
	if (playerState.WantsToLook)
	{
		RoomData currRoom = worldState.Rooms[playerState.CurrentRoomIndex];
		printf("=============================\n");
		printf("LOCATION: %s\n", currRoom.Name.c_str());
		printf("%s\n\n", currRoom.Description.c_str());
		
		if (currRoom.HasKey)
		{
			printf("Go and grab the key from the receptionist.\n\n");
		}

		if (currRoom.HasLadder)
		{
			printf("There's a ladder leaning against the wall.\n\n");
		}
		
		printf("EXITS: \n");
	
		for (unsigned int i = 0; i < currRoom.Exits.size(); ++i)
		{
			printf("%s\n", currRoom.Exits[i].Name.c_str());
		}

		if (playerState.HasKey)
		{
			printf("\nINVENTORY:\n");
			printf("Key\n");
		}

		if (playerState.HasLadder)
		{
			printf("\nINVENTORY:\n");
			printf("Ladder\n");
		}

		printf("\n");
	}

}

void UpdateGame(PlayerState& playerState, WorldState& worldState)
{
	RoomData currRoom = worldState.Rooms[playerState.CurrentRoomIndex];
	if (playerState.DesiredExit != "")
	{
		bool foundExit = false;

		for (unsigned int i = 0; i < currRoom.Exits.size(); ++i)
		{
			if (playerState.DesiredExit == currRoom.Exits[i].Name)
			{
				foundExit = true;

				if (!currRoom.Exits[i].Locked)
				{
					playerState.CurrentRoomIndex = currRoom.Exits[i].TargetRoomIndex;
					playerState.WantsToLook = true;
				}
				else
				{
					if (playerState.HasKey)
					{
						playerState.CurrentRoomIndex = currRoom.Exits[i].TargetRoomIndex;
						playerState.WantsToLook = true;
					}
					else
					{
						printf("It's locked.\n");
					}
				}
			}
		}
		if (!foundExit)
		{
			printf("No exit found with that name. \n\n");
		}

	}
	else if (playerState.DesiredPickup != "")
	{
		if (playerState.DesiredPickup == "key")
		{
			if (currRoom.HasKey)
			{
				printf("You acquired your home key!\n\n");
				playerState.HasKey = true;
				currRoom.HasKey = false;
			}
			else
			{
				printf("There's no key here.\n\n");
			}
		}
		
		else if (playerState.DesiredPickup == "ladder");
		{
			if (currRoom.HasLadder)
			{
				printf("You acquired a ladder!\n\n");
				playerState.HasLadder = true;
				currRoom.HasLadder = false;
			}
			
		}
		
	}
	else
	{
		printf("Can't find anything like that here.\n\n");
	}
	
}


void CleanupGame(PlayerState& playerState, WorldState& worldState)
{
	printf("Quitting...\n\n");
}