// Author: Alex Hartford
// Program: Emblem
// File: Commands
// Date: July 2022

/*
    TODO
    Specify the following cases of our FSM once things are further along.

    case(UNIT_MENU, UNIT_ATTACK, UNIT_HEAL, UNIT_TRADE, UNIT_ITEM, COMBAT,
            GAME_MENU_ROOT, GAME_MENU_OPTIONS, GAME_MENU_SAVE, 
            GAME_MENU_UNITS):
 */

#ifndef COMMAND_H
#define COMMAND_H

enum InterfaceState
{
    NEUTRAL_OVER_GROUND,
    NEUTRAL_OVER_ENEMY,
    NEUTRAL_OVER_UNIT,
    SELECTED_OVER_GROUND,
    SELECTED_OVER_INACCESSIBLE,
    SELECTED_OVER_ALLY,
    SELECTED_OVER_ENEMY,
    UNIT_MENU,
    UNIT_INFO,
    ENEMY_INFO,
    UNIT_ATTACK,
    UNIT_HEAL,
    COMBAT,
    UNIT_TRADE,
    UNIT_ITEM,
    GAME_MENU_ROOT,
    GAME_MENU_OPTIONS,
    GAME_MENU_SAVE,
    GAME_MENU_UNITS,
    NO_OP,
};

static InterfaceState GlobalInterfaceState = NEUTRAL_OVER_GROUND;



// =============================== commands ====================================
// inherited class, contains virtual methods.
class Command
{
public:
    virtual ~Command() 
    {
        //printf("Command has been Destructed!\n");
    }

    virtual void Execute() = 0;
};

class NullCommand : public Command
{
public:
    virtual void Execute()
    {
        printf("COMMAND | Null\n");
    }
};

// ============================ neutral mode commands ========================
class MoveCursorCommand : public Command
{
public:
    MoveCursorCommand(Cursor *cursor_in, int col_in, int row_in, const Tilemap &map_in)
    : cursor(cursor_in),
      col(col_in),
      row(row_in),
      map(map_in)
    {}

    virtual void Execute()
    {
        int newCol = cursor->col + col;
        int newRow = cursor->row + row;
        printf("COMMAND | Move Cursor from <%d, %d> to <%d, %d>\n", 
            cursor->col, cursor->row, newCol, newRow);

        if(IsValidBoundsPosition(newCol, newRow))
        {
            // move cursor
            cursor->col = newCol;
            cursor->row = newRow;

            // change state
            const Tile *hoverTile = &map.tiles[newCol][newRow];
            char buffer[256];

            if(!hoverTile->occupied)
            {
                GlobalInterfaceState = NEUTRAL_OVER_GROUND;
            }
            else
            {
                if(hoverTile->occupant->isAlly)
                {
                    GlobalInterfaceState = NEUTRAL_OVER_UNIT;
                }
                else
                {
                    GlobalInterfaceState = NEUTRAL_OVER_ENEMY;
                }
            }
        }
    }

private: 
    Cursor *cursor;
    int col;
    int row;
    const Tilemap &map;
};

class SelectUnitCommand : public Command
{
public:
    SelectUnitCommand(Cursor *cursor_in, Tilemap *map_in)
    : cursor(cursor_in),
      map(map_in)
    {}


    virtual void Execute()
    {
        GlobalInterfaceState = SELECTED_OVER_GROUND;
        cursor->selected = map->tiles[cursor->col][cursor->row].occupant;
        cursor->selectedCol = cursor->col;
        cursor->selectedRow = cursor->row;

        // Find tiles that a unit can access.
        map->accessible = AccessibleFrom(*map, cursor->selectedCol, cursor->selectedRow, 3);

        printf("COMMAND | Select Unit %d at <%d, %d>\n", 
               cursor->selected->id, cursor->col, cursor->row);
        printf(" <> INTERFACE | ally: %d, mov: %d, etc. <> \n",
               cursor->selected->isAlly, cursor->selected->mov);
    }

private:
    Cursor *cursor;
    Tilemap *map;
};



// ========================== selected mode commands =========================
class DeselectUnitCommand : public Command
{
public:
    DeselectUnitCommand(Cursor *cursor_in)
    : cursor(cursor_in)
    {}

    virtual void Execute()
    {
        printf("COMMAND | Deselect Unit %d.\n",
            cursor->selected->id);

        cursor->col = cursor->selectedCol;
        cursor->row = cursor->selectedRow;
        cursor->selected = nullptr;

        GlobalInterfaceState = NEUTRAL_OVER_UNIT;
    }

private:
    Cursor *cursor;
};


class MoveUpCheckedCommand : public Command
{
public:
    virtual void Execute() { printf("Move Up (Checked)!\n"); }
    // Check if Cursor is over accessible ground. Change State accordingly.
    // Check if Cursor over ground or target. Change state accordingly.
};

class MoveDownCheckedCommand : public Command
{
public:
    virtual void Execute() { printf("Move Down (Checked)!\n"); }
    // Check if Cursor is over accessible ground. Change State accordingly.
    // Check if Cursor over ground or target. Change state accordingly.
};

class MoveLeftCheckedCommand : public Command
{
public:
    virtual void Execute() { printf("Move Left (Checked)!\n"); }
    // Check if Cursor is over accessible ground. Change State accordingly.
    // Check if Cursor over ground or target. Change state accordingly.
};

class MoveRightCheckedCommand : public Command
{
public:
    virtual void Execute() { printf("Move Right (Checked)!\n"); }
    // Check if Cursor is over accessible ground. Change State accordingly.
    // Check if Cursor over ground or target. Change state accordingly.
};

class AttackEnemyCommand : public Command
{
public:
    virtual void Execute() { printf("Attack Enemy!\n"); }
    // Change state to Attack Preview State.
};

class HealAllyCommand : public Command
{
public:
    virtual void Execute() { printf("Heal Ally!\n"); }
    // Change state to Healing Preview State.
};

class BackDownFromHealingCommand : public Command
{
public:
    virtual void Execute() { printf("Backed down from Healing Ally!\n"); }
    // Change state to Selected Over Enemy State.
};

class InitiateHealingCommand : public Command
{
public:
    virtual void Execute() { printf("Initiate Healing!\n"); }
    // Change state to Combat State.
};

class ChangeWeaponBeforeCombatCommand : public Command
{
public:
    virtual void Execute() { printf("Change Weapon!\n"); }
};

class BackDownFromCombatCommand : public Command
{
public:
    virtual void Execute() { printf("Don't Attack Enemy!\n"); }
    // Change state to Selected Over Enemy State.
};

class InitiateCombatCommand : public Command
{
public:
    virtual void Execute() { printf("Initiate Combat!\n"); }
    // Change state to Combat State.
};

class PlaceUnitCommand : public Command
{
public:
    virtual void Execute() { printf("Place Unit!\n"); }
    // Change state to Selected Unit Actions State.
};

class SelectedItemsCommand : public Command
{
public:
    virtual void Execute() { printf("Selecting Items!\n"); }
    // Change state to Unit Items State.
};

class BackOutOfItemsCommand : public Command
{
public:
    virtual void Execute() { printf("Back Out of Items!\n"); }
    // Change state to Unit Actions State.
};

class SelectedTradeCommand : public Command
{
public:
    virtual void Execute() { printf("Trading Items!\n"); }
    // Change state to Unit Trade State.
};

class BackOutOfTradeCommand : public Command
{
public:
    virtual void Execute() { printf("Back Out of Trade!\n"); }
    // Change state to Unit Actions State.
};

class SelectedUnitInfoCommand : public Command
{
public:
    virtual void Execute() { printf("Reading Unit Info!\n"); }
    // Change state to Unit Info State.
};

class BackOutOfUnitInfoCommand : public Command
{
public:
    virtual void Execute() { printf("Back Out of Unit Info!\n"); }
    // Change state to Unit Actions State.
};



// ======================= selecting enemy commands ==========================
class SelectEnemyCommand : public Command
{
public:
    SelectEnemyCommand(Cursor *cursor_in, const Tilemap &map_in)
    : cursor(cursor_in),
      map(map_in)
    {}

    virtual void Execute()
    {
        GlobalInterfaceState = ENEMY_INFO;
        cursor->selected = map.tiles[cursor->col][cursor->row].occupant;
        cursor->selectedCol = cursor->col;
        cursor->selectedRow = cursor->row;

        printf("COMMAND | Select Enemy %d at <%d, %d>\n", 
            cursor->selected->id, cursor->col, cursor->row);
    }

private:
    Cursor *cursor;
    const Tilemap &map;
};

class DeselectEnemyCommand : public Command
{
public:
    DeselectEnemyCommand(Cursor *cursor_in)
    : cursor(cursor_in)
    {}

    virtual void Execute()
    {
        printf("COMMAND | Deselect Enemy %d.\n",
            cursor->selected->id);

        cursor->col = cursor->selectedCol;
        cursor->row = cursor->selectedRow;
        cursor->selected = nullptr;

        GlobalInterfaceState = NEUTRAL_OVER_ENEMY;
    }

private:
    Cursor *cursor;
};

class NextPageCommand : public Command
{
public:
    virtual void Execute() { printf("Next Page!\n"); }
    // Change state a little bit? Or just go to next page.
};

// ======================= player menu commands ==============================

class OpenMenuCommand : public Command
{
public:
    virtual void Execute()
    { 
        printf("COMMAND | Open Menu!\n");
    }
};

// MORE MENU COMMANDS GO HERE



// ============================== Input Handler ================================

class InputHandler
{
public:
    // abstraction layer.
    // simply executes the given command.
    shared_ptr<Command> HandleInput(InputState *input)
    {
        if(input->up) {
            input->up = false;
            return buttonUp;
        }
        if(input->down) {
            input->down = false;
            return buttonDown;
        }
        if(input->left) {
            input->left = false;
            return buttonLeft;
        }
        if(input->right) {
            input->right = false;
            return buttonRight;
        }
        if(input->a) {
            input->a = false;
            return buttonA;
        }
        if(input->b) {
            input->b = false;
            return buttonB;
        }

        return NULL;
    }

    // helper functions to bind Commands to each button.
    void BindUp(shared_ptr<Command> command)
    {
        buttonUp = command;
    }
    void BindDown(shared_ptr<Command> command)
    {
        buttonDown = command;
    }
    void BindLeft(shared_ptr<Command> command)
    {
        buttonLeft = command;
    }
    void BindRight(shared_ptr<Command> command)
    {
        buttonRight = command;
    }
    void BindA(shared_ptr<Command> command)
    {
        buttonA = command;
    }
    void BindB(shared_ptr<Command> command)
    {
        buttonB = command;
    }

    // updates what the user can do with their buttons.
    // contains some state, which is the minimum amount.
    // each individual command takes only what is absolutely necessary for its completion.
    void UpdateCommands(Cursor *cursor, Tilemap *map)
    {
        switch(GlobalInterfaceState)
        {
            case(NEUTRAL_OVER_GROUND):
            {
                BindUp(make_shared<MoveCursorCommand>(cursor, 0, -1, *map));
                BindDown(make_shared<MoveCursorCommand>(cursor, 0, 1, *map));
                BindLeft(make_shared<MoveCursorCommand>(cursor, -1, 0, *map));
                BindRight(make_shared<MoveCursorCommand>(cursor, 1, 0, *map));
                BindA(make_shared<OpenMenuCommand>());
                BindB(make_shared<NullCommand>());
            } break;

            case(NEUTRAL_OVER_ENEMY):
            {
                BindUp(make_shared<MoveCursorCommand>(cursor, 0, -1, *map));
                BindDown(make_shared<MoveCursorCommand>(cursor, 0, 1, *map));
                BindLeft(make_shared<MoveCursorCommand>(cursor, -1, 0, *map));
                BindRight(make_shared<MoveCursorCommand>(cursor, 1, 0, *map));
                BindA(make_shared<SelectEnemyCommand>(cursor, *map));
                BindB(make_shared<NullCommand>());
            } break;

            case(NEUTRAL_OVER_UNIT):
            {
                BindUp(make_shared<MoveCursorCommand>(cursor, 0, -1, *map));
                BindDown(make_shared<MoveCursorCommand>(cursor, 0, 1, *map));
                BindLeft(make_shared<MoveCursorCommand>(cursor, -1, 0, *map));
                BindRight(make_shared<MoveCursorCommand>(cursor, 1, 0, *map));
                BindA(make_shared<SelectUnitCommand>(cursor, map));
                BindB(make_shared<NullCommand>());
            } break;

            case(SELECTED_OVER_GROUND):
            {
                BindUp(make_shared<MoveUpCheckedCommand>());
                BindDown(make_shared<MoveDownCheckedCommand>());
                BindLeft(make_shared<MoveLeftCheckedCommand>());
                BindRight(make_shared<MoveRightCheckedCommand>());
                BindA(make_shared<PlaceUnitCommand>());
                BindB(make_shared<DeselectUnitCommand>(cursor));
            } break;

            case(SELECTED_OVER_INACCESSIBLE):
            {
                BindUp(make_shared<MoveUpCheckedCommand>());
                BindDown(make_shared<MoveDownCheckedCommand>());
                BindLeft(make_shared<MoveLeftCheckedCommand>());
                BindRight(make_shared<MoveRightCheckedCommand>());
                BindA(make_shared<NullCommand>());
                BindB(make_shared<DeselectUnitCommand>(cursor));
            } break;

            case(SELECTED_OVER_ALLY):
            {
                BindUp(make_shared<MoveUpCheckedCommand>());
                BindDown(make_shared<MoveDownCheckedCommand>());
                BindLeft(make_shared<MoveLeftCheckedCommand>());
                BindRight(make_shared<MoveRightCheckedCommand>());
                BindA(make_shared<HealAllyCommand>());
                BindB(make_shared<DeselectUnitCommand>(cursor));
            } break;


            case(SELECTED_OVER_ENEMY):
            {
                BindUp(make_shared<MoveUpCheckedCommand>());
                BindDown(make_shared<MoveDownCheckedCommand>());
                BindLeft(make_shared<MoveLeftCheckedCommand>());
                BindRight(make_shared<MoveRightCheckedCommand>());
                BindA(make_shared<AttackEnemyCommand>());
                BindB(make_shared<DeselectUnitCommand>(cursor));
            } break;

            case(UNIT_INFO):
            {
                BindUp(make_shared<NullCommand>());
                BindDown(make_shared<NullCommand>());
                BindLeft(make_shared<NullCommand>());
                BindRight(make_shared<NullCommand>());
                BindA(make_shared<BackOutOfUnitInfoCommand>());
                BindB(make_shared<BackOutOfUnitInfoCommand>());
            } break;


            case(ENEMY_INFO):
            {
                BindUp(make_shared<NullCommand>());
                BindDown(make_shared<NullCommand>());
                BindLeft(make_shared<NullCommand>());
                BindRight(make_shared<NullCommand>());
                BindA(make_shared<DeselectEnemyCommand>(cursor));
                BindB(make_shared<DeselectEnemyCommand>(cursor));
            } break;


            // TODO: Specify once Menus are further along.
            case(UNIT_MENU, UNIT_ATTACK, UNIT_HEAL, UNIT_TRADE, UNIT_ITEM, COMBAT,
                    GAME_MENU_ROOT, GAME_MENU_OPTIONS, GAME_MENU_SAVE, 
                    GAME_MENU_UNITS):
            {
            } break;


            case(NO_OP):
            {
                BindUp(make_shared<NullCommand>());
                BindDown(make_shared<NullCommand>());
                BindLeft(make_shared<NullCommand>());
                BindRight(make_shared<NullCommand>());
                BindA(make_shared<NullCommand>());
                BindB(make_shared<NullCommand>());
            } break;


            default:
            {
                assert(!"Invalid Cursor State!\n");
            } break;
        }
    }

private:
    shared_ptr<Command> buttonUp;
    shared_ptr<Command> buttonDown;
    shared_ptr<Command> buttonLeft;
    shared_ptr<Command> buttonRight;
    shared_ptr<Command> buttonA;
    shared_ptr<Command> buttonB;
};



#endif
