//
// Created by chen on 2018/10/23.
//

#ifndef TRICKYTOWERSERVER_ROOM_H
#define TRICKYTOWERSERVER_ROOM_H

#include <string>
using namespace std;

class room {
private:
    string name;
    int connectAmount = 0;
    int host = -1;
    int player2 = -1;
    int player3 = -1;
public:
    int getConnectAmount() const;

    void setConnectAmount(int connectAmount);

    string getName() const;

    void setName(string name);

    int getHost() const;

    void setHost(int host);

    int getPlayer2() const;

    void setPlayer2(int player2);

    int getPlayer3() const;

    void setPlayer3(int player3);

    int addPlayer(int player);

    room();

    room(string name,int host);
};


#endif //TRICKYTOWERSERVER_ROOM_H
