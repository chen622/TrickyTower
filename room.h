//
// Created by chen on 2018/10/23.
//

#ifndef TRICKYTOWERSERVER_ROOM_H
#define TRICKYTOWERSERVER_ROOM_H


class room {
private:
    int id;
    char *name;
    int connectAmount;
    int host = -1;
    int player2 = -1;
    int player3 = -1;
public:
    int getConnectAmount() const;

    void setConnectAmount(int connectAmount);

    int getId() const;

    void setId(int id);

    char *getName() const;

    void setName(char *name);

    int getHost() const;

    void setHost(int host);

    int getPlayer2() const;

    void setPlayer2(int player2);

    int getPlayer3() const;

    void setPlayer3(int player3);

    int addPlayer(int player);

    room(int id, char *name,int host);
};


#endif //TRICKYTOWERSERVER_ROOM_H
