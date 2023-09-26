#include <iostream>
#include<iomanip>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include<vector>
#include "rapidjson/istreamwrapper.h"
#include "rapidjson/ostreamwrapper.h"
const char* pathPlayer="C:\\Users\\agnalraj-1\\Downloads\\final\\Project final\\Project\\player.json";
const char* pathBoard ="C:\\Users\\agnalraj-1\\Downloads\\final\\Project final\\Project\\board.json";
using namespace rapidjson;
class Match
{
    int initialAmount{};
    int initialPosition{};
public:
    //constructor initialization
    Match(int initialAmount=10000,int initialPosition=0):initialAmount{initialAmount},initialPosition{initialPosition}{
    }

    /*
        This function returns the Document file after 
        parsing of IstreamWrapper file to the Document file
    */
    Document get_Iwrapper(const char* path)
    {
        std::ifstream in_file{path};
        Document d;
        if(!in_file){
            std::cout<<"failed to open json file";
            return d;
        }

        IStreamWrapper isw(in_file);
        d.ParseStream(isw);

        if(d.HasParseError()){
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


    //function which adds the JSon value and add to the Json
    void AddJson(int numberOfPlayers)
    {
        Document d=get_Iwrapper(pathPlayer);
        Value player(kObjectType);
        for(int i=1;i<=numberOfPlayers;i++){
            player.SetObject();
            std::cout<<"Adding player "<<i<<std::endl;
            std::string name{};
            std::cout<<"Enter the player "<<i<<" name : ";
            std::cin>>name;
            int playerNumber = i;

            //adding the player key-pair
            std::string key{"player_"};
            key.append(std::to_string(playerNumber));

            player.AddMember("name",Value(name.c_str(), d.GetAllocator()).Move(),d.GetAllocator());
            player.AddMember("amount",initialAmount,d.GetAllocator());
            player.AddMember("Position",initialPosition,d.GetAllocator());
            player.AddMember("propertyamount",0,d.GetAllocator());

            d.AddMember(Value(key.c_str(), d.GetAllocator()).Move(),player.Move(),d.GetAllocator());

            //update the file
            set_Owrapper(pathPlayer,d);
        }
    }

    //copy the board data
    void copy_json(const char* sourcePath,const char* destinationPath)
    {
        std::ifstream sourcefile(sourcePath);
        if(!sourcefile){
            std::cout<<"failed to open source file during copy"<<std::endl;
            return;
        }

        IStreamWrapper isw(sourcefile);
        Document sourceDoc;
        sourceDoc.ParseStream(isw);

        std::ofstream destinationfile(destinationPath);
        if(!destinationfile){
            std::cout<<"Failed to destination json file during copy"<<std::endl;
            return;
        }
        OStreamWrapper osw(destinationfile);
        Writer<OStreamWrapper> Writer(osw);
        sourceDoc.Accept(Writer);
        sourcefile.close();
        destinationfile.close();
    }


    //returns the name of the player
    std::string get_name(int playerNumber) 
    {
        std::string playerName = "player_" + std::to_string(playerNumber);

        Document d=get_Iwrapper(pathPlayer);
        Value& playerObject = d[playerName.c_str()];
        Value& playerNameValue = playerObject["name"];
        return playerNameValue.GetString();
    }

    //function to clear the Previous Json data
    void clear_json(const char* deletePath)
    {
        std::ofstream op_file{deletePath};
        if(!op_file){
            std::cout<<"failed to open json file during clearing json";
            return;
        }
        Document doc;
        doc.SetObject();
        OStreamWrapper osw(op_file);
        Writer<OStreamWrapper> Writer(osw);
        doc.Accept(Writer);
        op_file.close();
    }


    //function to return the results of the player after the given number of rounds
    void display_results(int numberOfPlayers)
    {
        std::cout<<"\n\n\n\n"<<"+----------Results---------+"<<std::endl;
        std::vector<std::pair<std::string,int>> results;
        for(int i=1;i<=numberOfPlayers;i++)
        {
            std::string IdOfOwner ="player_"+std::to_string(i);
            Document d_player = get_Iwrapper(pathPlayer);
            Value &playerObject = d_player[IdOfOwner.c_str()];
            std::string playerName = playerObject["name"].GetString();
            int totalAmount = playerObject["amount"].GetInt()+playerObject["propertyamount"].GetInt();  
            results.push_back(std::make_pair(playerName,totalAmount));
        }

        //sorting the pair based the value of amount
        std::sort(results.begin(), results.end(),[](const std::pair<std::string,int>& a, const std::pair<std::string,int>& b) {
                  return a.second > b.second;
              });
        
        std::cout<<"1   "<<results[0].first<<"  "<<results[0].second<<std::endl;
        int position1 = 1;
        int position2 = 2;

        for(int i=1;i<results.size();i++)
        {
            int currAmount = results[i].second;
            int prevAmount = results[i-1].second;
            if(currAmount==prevAmount)
            {
                std::cout<<position1<<"   "<<results[i].first<<"  "<<results[i].second<<std::endl;
            }
            else
            {
                std::cout<<position2<<"   "<<results[i].first<<"  "<<results[i].second<<std::endl;
                position1=position2;
            }
            position2++;
        } 

        std::cout<<"+--------------------------+"<<std::endl<<std::endl<<std::endl;
    }


    //function which displays the board details to find about the cells in the board
    void display_board()
    {
        Document d_board=get_Iwrapper(pathBoard);
        Document d_player=get_Iwrapper(pathPlayer);

        for(int i=0;i<=37;i++)
        {
            std::string boardId = "cell_"+std::to_string(i);
            std::string place=d_board[boardId.c_str()]["place"].GetString();
            std::string price;
            std::string owner;
            if(place=="Jackpot!!" || place=="start" || place=="LuckyJump!!" || place=="Jail:(")
            {
                price="NA";
                owner="NA";
            }
            else
            {
                price = std::to_string(d_board[boardId.c_str()]["price"].GetInt());
                Value &v = d_board[boardId.c_str()]["owner"];
                std::string playerId{};
                if(!v.IsNull()) playerId = v.GetString();
                if(!playerId.empty())owner = d_player[playerId.c_str()]["name"].GetString();
                else owner = "Not sold";
            }

            std::cout << "+------------------------------------+" << std::endl;
            std::cout << "|"<<std::setw(20)<< boardId << std::setw(17) <<"|" << std::endl;
            std::cout << "+------------------------------------+" << std::endl;
            std::cout<<"       place : "<<place<<std::endl;
            std::cout<<"       price : "<<price<<std::endl;
            std::cout<<"       owner : "<<owner<<std::endl;
            std::cout << "+------------------------------------+" << std::endl<<std::endl;

        }
    }

    //Function to return the balance of the Player
    int show_balance(int playerNumber)
    {
        Document d=get_Iwrapper(pathPlayer);
        std::string playerName="player_"+std::to_string(playerNumber);
        return d[playerName.c_str()]["amount"].GetInt(); 
    }
};