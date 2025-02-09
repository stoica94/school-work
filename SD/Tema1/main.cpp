/* Copyright 2018 <Stoica George-Ovidiu>
  Copy this a baby duck will cry */
#include<iostream>
#include<fstream>
#include<string>
#include<tuple>
#include<cstring>
#include "Robot.h"

/* List of all the types of commands */
enum string_command {
    ADD,
    UNDO,
    EXECUTE,
    PRINT_COMMANDS,
    LAST_EXECUTED_COMMAND,
    HOW_MUCH_TIME,
    HOW_MANY_BOXES,
    DEFAULT
};

/* Function used for switch statement */
string_command interpretCommand(std::string command) {
    if (command == "ADD_GET_BOX" || command == "ADD_DROP_BOX") return ADD;
    if (command == "UNDO") return UNDO;
    if (command == "EXECUTE") return EXECUTE;
    if (command == "PRINT_COMMANDS") return PRINT_COMMANDS;
    if (command == "LAST_EXECUTED_COMMAND") return LAST_EXECUTED_COMMAND;
    if (command == "HOW_MUCH_TIME") return HOW_MUCH_TIME;
    if (command == "HOW_MANY_BOXES") return HOW_MANY_BOXES;
    return DEFAULT;
}

/* Checks if the command uses a robot ID */
bool usesID(std::string command) {
    string_command interp = interpretCommand(command);
    if (interp == UNDO || interp == HOW_MUCH_TIME ||
            interp == LAST_EXECUTED_COMMAND) return false;
    return true;
}

int main() {
    std::ifstream file("robots.in");
    std::string command;
    int N, LIN, COL;

    std::ofstream out("robots.out");  // Redirecting cout to file
    std::streambuf *coutbuf = std::cout.rdbuf();
    std::cout.rdbuf(out.rdbuf());


    // Stack used for UNDO
    Stack<std::tuple<int, Command>> history;

    // Stack used for LAST EXECUTED COMMANDS
    Stack<std::tuple<int, Command>> executedCommands;

    // Reading initial parameters
    file >> N >> LIN >> COL;
    int **map = new int*[LIN];
    for (int i = 0; i < LIN; i++) {
        map[i] = new int[COL];
    }

    // Reading the matrix from file
    for (int i = 0; i < LIN; i++) {
        for (int j = 0; j < COL; j++) {
            file >> map[i][j];
        }
    }

    // The robots on the map
    Robot *robots = new Robot[N];
    for (int i = 0; i < N; i++) {
        robots[i].setID(i);
    }


    // Ignoring last parts of the matrix line
    file.ignore(5000, '\n');

    while (std::getline(file, command)) {
        if (command == ""){
          break;
        }

        Command buf;  // Bad practice to declare inside Switch statement

        int pos = 0;
        int ID = 0;
        std::string firstToken = command.substr(0, command.find(' '));

        if (usesID(firstToken)) {
            pos = command.find(" ");
        }

        // Splitted what is after the command name
        std::string s = command.substr(pos);
        std::stringstream ss(s);

        switch (interpretCommand(firstToken)) {
        // ADD_GET or ADD_DROP Command
        case ADD:
            ss >> ID;
            buf.type = command.at(4);  // G or D
            ss >> buf.x >> buf.y >> buf.nr_boxes >> buf.priority;
            buf.exTime = 0;
            robots[ID].addCommand(buf);
            history.push(std::make_tuple(ID, buf));
            break;

        // EXECUTE Command
        case EXECUTE:
            ss >> ID;
            buf = robots[ID].executeCommand(map);

            // If there was nothing to execute
            if (buf.type == '0') {
                std::cout << "EXECUTE: No command to execute";
                std::cout << std::endl;
                break;
            }

            executedCommands.push(std::make_tuple(ID, buf));

            buf.type = 'E';  // Set type to Execute
            history.push(std::make_tuple(ID, buf));
            break;

        // UNDO Command
        case UNDO:
            if (history.isEmpty()) {
                std::cout << "UNDO: No history" << std::endl;
                break;
            }
            ID = std::get<0>(history.peek());

            // Last command is Execute
            if (std::get<1>(history.peek()).type == 'E') {
                buf = std::get<1>(executedCommands.peek());
                robots[ID].undoCommand(&buf, map);
                executedCommands.pop();
            } else {
                buf = std::get<1>(history.peek());
                robots[ID].popLastCommand(buf.priority);
            }
            history.pop();
            break;

        // PRINT Command
        case PRINT_COMMANDS:
            ss >> ID;
            std::cout << "PRINT_COMMANDS: ";
            if (robots[ID].noCommands()){
              std::cout << "No command found" << std::endl;
              break;
            }
            robots[ID].printCommands();
            std::cout << std::endl;
            break;

        // HOW MUCH TIME Command
        case HOW_MUCH_TIME:
            std::cout << "HOW_MUCH_TIME: ";
            if (executedCommands.isEmpty()) {
                std::cout << "No command was executed" << std::endl;
                break;
            }
            buf = std::get<1>(executedCommands.peek());
            std::cout << buf.exTime << std::endl;
            break;

        // HOW_MANY_BOXES Command
        case HOW_MANY_BOXES:
            ss >> ID;
            std::cout << "HOW_MANY_BOXES: ";
            std::cout << robots[ID].getBoxes() << std::endl;
            break;

        // LAST_EXECUTED_COMMAND Command
        case LAST_EXECUTED_COMMAND:
            std::cout << "LAST_EXECUTED_COMMAND: ";
            if (executedCommands.isEmpty()) {
                std::cout << "No command was executed" << std::endl;
                break;
            }

            ID = std::get<0>(executedCommands.peek());
            buf = std::get<1>(executedCommands.peek());
            std::cout<< ID << ": ";
            std::cout << buf << std::endl;
            break;

        // UNRECOGNIZED Command
        case DEFAULT:
            std::cout <<"UNKNOWN_COMMAND" << std::endl;
            break;
        }

        std::memset(&buf, 0, sizeof(buf));  // Clearing buffer

        // Updating time for all robots
        for (int i = 0; i < N; i++) {
            robots[i].updateTime();
        }
    }

    // Clearing memory
    for (int i = 0; i < LIN; i++) {
        delete [] map[i];
    }
    delete [] map;
    delete [] robots;

    std::cout.rdbuf(coutbuf);  // Settng cout back to stdout
    return 0;
}
