//
// Created by chen on 2018/10/23.
//

#include "room.h"

room::room(int id, char *name, int host) : id(id), name(name), host(host) {}

int room::getId() const {
    return id;
}

void room::setId(int id) {
    room::id = id;
}

char *room::getName() const {
    return name;
}

void room::setName(char *name) {
    room::name = name;
}

int room::getHost() const {
    return host;
}

void room::setHost(int host) {
    room::host = host;
}

int room::getPlayer2() const {
    return player2;
}

void room::setPlayer2(int player2) {
    room::player2 = player2;
}

int room::getPlayer3() const {
    return player3;
}

void room::setPlayer3(int player3) {
    room::player3 = player3;
}

int room::addPlayer(int player) {
    if (getConnectAmount() == 3)
        return 0;
    else if (player2 == -1) {
        player2 = player;
        return 2;
    } else if(player3 == -1){
        player3 = player;
        return 3;
    }
}



int room::getConnectAmount() const {
    return connectAmount;
}

void room::setConnectAmount(int connectAmount) {
    room::connectAmount = connectAmount;
}
