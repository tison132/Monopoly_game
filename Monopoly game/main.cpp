#include<iostream>
#include<math.h>
#include "Match.cpp"
#include "Dice.cpp"
const char* sourcePathBoard = "C:\\Users\\agnalraj-1\\Downloads\\final\\Project final\\Project\\Board_data.json";
const char* destinationPathBoard = "C:\\Users\\agnalraj-1\\Downloads\\final\\Project final\\Project\\board.json";
int main() {
    int numberOfPlayers{};
    std::cout<<"Enter the total number of players: ";
    

    //infinite loop with try catch to get the valid input
    while(1){
        std::cin>>numberOfPlayers;
        try {
            if (!(numberOfPlayers) || numberOfPlayers<2 || numberOfPlayers>6) {
                throw std::invalid_argument("Invalid input. Please enter above 1 and below 7.");
            }
            break;
        } 
        catch (const std::invalid_argument& e) {
            std::cerr << "Error: " << e.what() << std::endl;
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }

    //object creation
    Match play_match;
    Dice roll_dice;

    //clear the older file of board
    play_match.clear_json(destinationPathBoard);
    //clear the older file of player
    play_match.clear_json(pathPlayer);

    //function call to get the player data and add to json file
    play_match.AddJson(numberOfPlayers);

    //copying the board data
    play_match.copy_json(sourcePathBoard,destinationPathBoard);
    play_match.display_board();
    //Total round of rounds in the match
    const int numberOfRounds=10;
    int roundCount=1;
    while (roundCount<=numberOfRounds)
    {
        std::cout<<"---------------------Round "<<roundCount<<" starts----------------------------"<<std::endl;
        for(int i=1;i<=numberOfPlayers;i++){
            std::cout<<play_match.get_name(i)<<"'s turn to play"<<std::endl;
            std::cout<<"Press any key to roll the dice : ";
            std::string userInput{};
            std::cin>>userInput;

            //generate a dice value using random generator function
            int randomValue = roll_dice.generate_random(2,12);
            //get the blance of the user
            int amount = play_match.show_balance(i);
            std::cout<<"**************************************"<<std::endl;
            std::cout << std::string(24, '-') << std::endl;
            std::cout << "| " << "Dice value" << " | " << "Balance" << " |" << std::endl;
            std::cout << std::string(24, '-') << std::endl;
            std::cout << "| "<<std::setw(9) << randomValue << " | " <<std::setw(8)<< amount << " |" << std::endl;
            std::cout << std::string(24, '-') << std::endl;
            
            //handle the movement of the cell
            roll_dice.handle_move(randomValue,i);
            //display the account balance after the move
            std::cout<<"Your Current Balance:"<<play_match.show_balance(i)<<std::endl;
            std::cout<<"**************************************"<<std::endl<<std::endl<<std::endl;
        }
        std::cout<<"---------------------Round "<<roundCount<<" ends----------------------------"<<std::endl<<std::endl<<std::endl;
        roundCount++;
    }
    std::cout<<std::setw(15)<<"-----------------Game over!------------------"<<std::setw(15)<<std::endl;

    //display the results of the user
    play_match.display_results(numberOfPlayers);
    
    std::string choice{};
    std::cout<<"Whether you want to display the board details:"<<std::endl;
    std::cout<<"Press (Y for Yes) and (any key for No):";
    std::cin>>choice;
    //display the details of the board
    if(choice=="y" || choice == "Y")
        play_match.display_board();
}