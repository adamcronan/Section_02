/*
This is the console exe that makes use of the BullCow class
This acts as the view in a MVC pattern, and is responsible for all
user interaction. For game logic see the FBullCowGame class.
*/
#pragma once
#include <iostream>
#include <string>
#include <ctime>
#include "FBullCowGame.h"

// To make syntax Unreal friendly
using FText = std::string;
using int32 = int; 

// Function prototypes as outside a class
void PrintGameTitlePicture();
void PrintGameDirections();
void PrintIntroAndSetWord();
void InitializeWord();
void InitializeWord(int32);
void PrintGuessSummary(FBullCowCount);
void PlayGame();
void PrintRoundSummary();
void PrintGameWinSummary();
void PrintGameLossSummary(bool);
FText GetValidGuess();
bool AskToKeepPlaying();
bool CheckToContinuePlay(); 

FBullCowGame BCGame;				// Instantiate a new game which we reuse across plays

int main()							// Entry point for our application
{
	srand(time(NULL));				// Seed random number generator with new value on every run
	BCGame.ResetPlayerPointTotal();	// Initialize/Reset point total to 0 on game initialization
	BCGame.SetGameCompletionMap();	// Called once at beginning of game to keep track of which word-lengths user has guessed

	bool bPlayAgain = false; 
	do {
		PrintIntroAndSetWord();
		PlayGame();
		bPlayAgain = CheckToContinuePlay(); 
	} 
	while (bPlayAgain);
	
	return 0;						// Exit the application
}

void PrintIntroAndSetWord() // Introduce game and set word
{
	PrintGameTitlePicture();
	if (!BCGame.GetEnterBonusRound()) { InitializeWord(); }
	else { InitializeWord(BCGame.GetBonusWordLength()); }
	PrintGameDirections(); 
	std::cout << std::endl;

	return;
}

void PrintGameTitlePicture()
{
	std::cout << "-------------------------------------------\n";
	std::cout << "Welcome to Bulls and Cows, a fun word game!\n";
	std::cout << "-------------------------------------------\n";
	std::cout << std::endl;
	std::cout << "          }   {         ___ " << std::endl;
	std::cout << "          (o o)        (o o) " << std::endl;
	std::cout << "   /-------\\ /          \\ /-------\\ " << std::endl;
	std::cout << "  / | BULL |O            O| COW  | \\ " << std::endl;
	std::cout << " *  |-,--- |              |------|  * " << std::endl;
	std::cout << "    ^      ^              ^      ^ " << std::endl;
	std::cout << std::endl;
	return;
}

void InitializeWord()								// Get the current word length and set the word (and Maps) based on length
{
	int32 WordLength = BCGame.GetCurrentWordLength();
	BCGame.SetHiddenWordAndLength(WordLength);
	BCGame.SetPointMaps(BCGame.GetHiddenWord());
	return;
}

void InitializeWord(int32 number)
{
	BCGame.SetHiddenWordAndLength(number);
	BCGame.SetPointMaps(BCGame.GetHiddenWord());
	return;
}

void PrintGameDirections()
{
	std::cout << " << Can you guess the " << BCGame.GetHiddenWordLength();
	std::cout << " letter isogram I'm thinking of?? >>\n";
	std::cout << " ----------------------------------------------------------\n";
	std::cout << " << A Bull means a correct letter in the correct place.  >>\n";
	std::cout << " << A Cow means a correct letter in the incorrect place. >>\n";
	std::cout << " ----------------------------------------------------------\n";
	std::cout << " <<     If you get a Bull, watch for Helpful Hints!!     >>\n";
	std::cout << " ----------------------------------------------------------\n";
	std::cout << "                <<<<< Have fun!!!! >>>>>\n";
	std::cout << BCGame.GetHiddenWord() << std::endl;
	return;
}

void PlayGame()
{
	BCGame.Reset();

	int32 MaxTries = BCGame.GetMaxTries(); 
	while (!BCGame.IsWordGuessed() && BCGame.GetCurrentTry() <= MaxTries)	// Loop asking for guesses while word is not guessed and while having tries remaining
	{
		FText Guess = GetValidGuess();
		FBullCowCount BullCowCount = BCGame.SubmitValidGuess(Guess);		// Submit valid guess to the game
		PrintGuessSummary(BullCowCount);
	}

	if (BCGame.IsWordGuessed()) { BCGame.ImplementWordWinCondition(); }		// Player guessed the correct word in a round
	else { BCGame.ImplementWordLossCondition(); }							// Player did not guess the word

	return; 
}

void PrintGuessSummary(FBullCowCount BullCowCount)							// Shows Player the number of Bulls and Cows in guess
{																			// and prints a hint for the bulls
	std::cout << "Bulls = " << BullCowCount.Bulls;
	std::cout << ". Cows = " << BullCowCount.Cows << ".\n";
	std::cout << "Helpful hint: In your guess, the letters { " << BCGame.GetGameHelper();
	std::cout << " } are in the correct position.\n";
	std::cout << "Current points: " << BCGame.GetMyPoints() << "\n\n";
	return;
}

FText GetValidGuess()						// Makes sure player puts in a valid string for the game
{
	FText Guess = "";
	EGuessStatus Status = EGuessStatus::Invalid_Status;

	do {
		BCGame.ResetGameHelper(); 
		int32 CurrentTry = BCGame.GetCurrentTry();
		std::cout << "Try " << CurrentTry << " of " << BCGame.GetMaxTries();
		std::cout << ". Enter your guess: ";
		std::getline(std::cin, Guess);		// Get guess from player 

		Status = BCGame.CheckGuessValidity(Guess);
		switch (Status)
		{
		case EGuessStatus::Incorrect_Length:
			std::cout << "Please enter a " << BCGame.GetHiddenWordLength() << " letter word.\n\n";
			break;
		case EGuessStatus::Not_Isogram:
			std::cout << "Word entered was not an isogram!\n\n";
			break;
		case EGuessStatus::Not_Lowercase:
			std::cout << "Please enter only lowercase letters.\n\n";
			break;
		default:							// If code gets here, assume Guess is valid
			break;
		}
	} while (Status != EGuessStatus::OK);	// Keep looping until input is valid

	return Guess;
	 
}

bool CheckToContinuePlay()												// Check for correct conditions to continue game
{
	bool bPositivePoints = (BCGame.GetMyPoints() > 0);					// TODO make calculation for more robust check for negative value
	bool bGameIsWon = BCGame.IsGameWon();
	if (bPositivePoints && BCGame.GetCurrentWordLength() <= BCGame.GetMaxWordLength())	// Keep going if user has positive points
	{
		PrintRoundSummary();
		std::cout << "On to the next word? (Y/n) ";
		return AskToKeepPlaying();
	}
	else					
	{
		if (bGameIsWon && bPositivePoints)							// Win check
		{ 
			PrintGameWinSummary();
			if (BCGame.GetRestartAfterBonus())						// This is true if player wants to play after completing the bonus round
			{														// Wont be true the first time player reaches endgame
				BCGame.ResetPlayerPointTotal();
				BCGame.ResetCurrentWordLength();
				BCGame.SetEnterBonusRound(false);					// Resetting variables to play a new game
				BCGame.SetRestartAfterBonus(false);
				return true;
			}
			else if (BCGame.GetEnterBonusRound()) { return true; }	// This is true if player has completed main game and wants to enter bonus round
			else { return false; }
		}
		else { 
			PrintGameLossSummary(bPositivePoints);					// Player loses if above check is false
			if (AskToKeepPlaying()) {
				BCGame.ResetPlayerPointTotal();
				BCGame.ResetCurrentWordLength();
				return true;
			}
			else { return false; }
		}
	}
}

void PrintRoundSummary()								// States the result of the round just played, including points
{
	int32 FinalPointValue = BCGame.GetMyPoints();
	std::cout << "Total points: " << FinalPointValue << std::endl;
	std::cout << std::endl;
	if (BCGame.IsWordGuessed()) { std::cout << "WELL DONE!!\n\n"; }
	else { std::cout << "Better luck next time!\n\n"; }
	return;
}

bool AskToKeepPlaying()									// Gets a player response and interprets it to proceed
{
	FText Response = "";
	std::getline(std::cin, Response);
	return (Response[0] == 'Y' || Response[0] == 'y' || Response == "");
}

void PrintGameWinSummary()
{
	if (!BCGame.GetEnterBonusRound()) {
		std::cout << "Final points: " << BCGame.GetMyPoints() << std::endl;
		std::cout << std::endl;
		std::cout << "=====================================\n";
		std::cout << "YOU WON THE GAME! CONGRATULATIONS!!!!\n";
		std::cout << "=====================================\n\n";
		std::cout << "Would you like to enter the bonus round? (Y/n) \n";
		FText Response = "";
		std::getline(std::cin, Response);
		BCGame.SetEnterBonusRound((Response[0] == 'Y' || Response[0] == 'y' || Response == ""));
	}
	else
	{
		BCGame.SetCompleteBonusRound(true);
		std::cout << "Final points: " << BCGame.GetMyPoints() << std::endl;
		std::cout << std::endl;
		std::cout << "========================================\n";
		std::cout << "AMAZING! YOU COMPLETED THE BONUS ROUND!!\n";
		std::cout << "========================================\n\n";
		std::cout << "You destroyed my game! Would you like to restart? (Y/n)";
		FText Response = "";
		std::getline(std::cin, Response);
		BCGame.SetRestartAfterBonus((Response[0] == 'Y' || Response[0] == 'y' || Response == ""));
	}
	return;
}

void PrintGameLossSummary(bool PositivePoints)
{
	std::cout << "Final points: " << BCGame.GetMyPoints() << std::endl;
	std::cout << "\n====================================================\n";
	if (PositivePoints) { std::cout << "YOU LOST, you did not guess all of the words!\n"; }
	else { std::cout << "YOU LOST, you did not have enough points to continue.\n"; }
	std::cout << "====================================================\n\n";
	std::cout << "Would you like to restart? (Y/n) \n";
	return;
}
