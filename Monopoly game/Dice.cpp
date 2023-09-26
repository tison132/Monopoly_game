#include<iostream>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <iostream>
#include<fstream>
#include "rapidjson/istreamwrapper.h"
#include "rapidjson/ostreamwrapper.h"
#include <random>
using namespace rapidjson;
class Dice
{
public:

    //Function to generate the Random number between lowerBound and upperBound
    int generate_random(int lowerBound,int upperBound)
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<int> distribution(lowerBound, upperBound);
        int random_number = distribution(gen);
        return random_number;
    }

    /*
        This function returns the Document file after 
        parsing of IstreamWrapper file to the Document file
    */
    Document get_Iwrapper(const char* path)
    {
        std::ifstream in_file{path};
        Document d;
        if(!in_file)
        {
            std::cout<<"failed to open json file";
            return d;
        }

        IStreamWrapper isw(in_file);
        d.ParseStream(isw);

        if(d.HasParseError())
        {
            std::cout<<"Has parse error"<<std::endl;
            return d;
        }
        return d;
    }

    //This function helps to made change in the orginal file
    void set_Owrapper(const char* path,const Document& doc)
    {
            std::ofstream op_file{path};
            OStreamWrapper osw(op_file);
            Writer<OStreamWrapper> writer(osw);
            doc.Accept(writer);
            op_file.close();
    }

    //function helps to handle the movement based on dice value
    void handle_cell(int PositionOfPlayer, int playerNumber) 
    {
        Document d_Board = get_Iwrapper(pathBoard);
        Document d_player = get_Iwrapper(pathPlayer);

        std::string cellName = "cell_" + std::to_string(PositionOfPlayer);
        Value& cellObject = d_Board[cellName.c_str()];
        std::string place = cellObject["place"].GetString();
        
        std::string PlayerName = "player_" + std::to_string(playerNumber);
        Value& PlayerObject = d_player[PlayerName.c_str()];

        if (place == "Jail:(") 
        {
            std::cout << "Oh no! you landed in Jail" << std::endl;
            int randomValue = generate_random(100, 300);
            std::cout << "You have to pay the fine of amount of $: " << randomValue << std::endl;

            Value& v = PlayerObject["amount"];
            int currentAmount = v.GetInt();
            currentAmount -= randomValue;   
            v.SetInt(currentAmount);        
            set_Owrapper(pathPlayer, d_player);
        }
        else if (place == "Jackpot!!") 
        {
            std::cout << "yayy! you landed in Jackpot" << std::endl;
            int randomValue = generate_random(100, 300);
            std::cout << "You have won the Jackpot! You get an extra amount of $: " << randomValue << std::endl;

            Value& v = PlayerObject["amount"];
            int currentAmount = v.GetInt(); 
            currentAmount += randomValue;   
            v.SetInt(currentAmount);        
            set_Owrapper(pathPlayer, d_player);
        }
        else if (place == "start") 
        {
            std::cout << "You landed in at start" << std::endl;
        }
        else if (place == "LuckyJump!!") 
        {
            std::cout << "Hurrah! you got an extra Move!!!" << std::endl;
            std::cout << "Press any key to roll the dice : ";
            std::string userInput{};
            std::cin >> userInput;
            std::cout << std::endl;
            int diceValue = generate_random(2, 12);
            handle_move(diceValue, playerNumber);
        }
      
        else 
        {
            std::cout<<"you landed in "<<place<<std::endl;
             int price = cellObject["price"].GetInt();
             int Amount = PlayerObject["amount"].GetInt();
            if(cellObject["owner"].IsNull())
            {
                std::cout<<"price of "<<place<<" : "<<price<<std::endl;
                std::string choice{"Y"};
                std::cout<<"Would you like to buy "<<place<<std::endl;
                std::cout<<"Press Y for Yes and Any key for No : ";
                std::cin>>choice;
                if(choice=="y" || choice=="Y")
                {
                    if(Amount>=price)
                    {
                        Value &playerAmount=PlayerObject["amount"];
                        playerAmount.SetInt(Amount-price);
                        Value &playerPropertyAmount=PlayerObject["propertyamount"];
                        playerPropertyAmount.SetInt(playerPropertyAmount.GetInt()+price);
                        set_Owrapper(pathPlayer,d_player);
                        std::string nameOfOwner ="player_"+std::to_string(playerNumber);
                        cellObject["owner"].SetString(nameOfOwner.c_str(),d_Board.GetAllocator());
                        set_Owrapper(pathBoard,d_Board);
                        std::cout<<"Congratulations! you successfully purchased "<<place<<std::endl;
                    }
                    else 
                    {
                        std::cout<<"Sorry you don't have enough money to buy this place"<<std::endl;
                    }
                }
                else{
                    std::cout<<"Okay!! Better luck for new places next time"<<std::endl;
                }
            }

            else
            {
                std::string ownerID=cellObject["owner"].GetString();
                std::string nameOfOwner ="player_"+std::to_string(playerNumber);
                if(ownerID==nameOfOwner)
                {
                    std::cout<<"You landed in your own place"<<std::endl;
                    if(Amount<500)
                    {
                        std::cout<<"You won't have enough money to upgrade your place "<<std::endl;
                        return;
                    }
                    std::cout<<"Would you like to upgrade (Y-for Yes/Any key-for No) ?"<<std::endl;
                    std::string choice;
                    std::cin>>choice;
                    if(choice=="y" || choice=="Y")
                    {
                        cellObject["price"].SetInt(cellObject["price"].GetInt()+500);
                        PlayerObject["amount"].SetInt(Amount-500);
                        Value &playerPropertyAmount=PlayerObject["propertyamount"];
                        playerPropertyAmount.SetInt(playerPropertyAmount.GetInt()+500);
                        set_Owrapper(pathPlayer,d_player);
                        set_Owrapper(pathBoard,d_Board);
                    }

                }
                else
                {
                    std::cout<<"This land is already owned by "<<(d_player[ownerID.c_str()]["name"]).GetString()<<std::endl;
                    int rent=price/5;
                    std::cout<<"The rent amount of $"<<rent<<" will be debited from your account"<<std::endl;
                    PlayerObject["amount"].SetInt(Amount-rent);
                    Value &owneramount=d_player[ownerID.c_str()]["amount"];
                    owneramount.Set(owneramount.GetInt()+rent);
                    set_Owrapper(pathPlayer,d_player);
                }
            }
        }   
    }

    //function helps in movement of position in the cell
    void handle_move(int diceValue,int playerNumber)
    {
        Document d=get_Iwrapper(pathPlayer);
        std::string playerName = "player_" + std::to_string(playerNumber);
        Value& playerObject = d[playerName.c_str()];
        Value &v=playerObject["Position"];
        int PositionOfPlayer = v.GetInt()+diceValue;
        
        if(PositionOfPlayer>=38)
        {
            std::cout<<"You've Successfully crossed the board once! A bonus of $500 to you"<<std::endl;
            PositionOfPlayer=PositionOfPlayer%38;
            Value &v=playerObject["amount"];
            v.SetInt(v.GetInt()+500);
            set_Owrapper(pathPlayer,d);
        }

        std::cout<<"Your current Position is "<<PositionOfPlayer<<std::endl;
        v.SetInt(PositionOfPlayer);
        set_Owrapper(pathPlayer,d);
        handle_cell(PositionOfPlayer,playerNumber);
    }
};