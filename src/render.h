// Author: Alex Hartford
// Program: Emblem
// File: Render
// Date: July 2022

#ifndef RENDER_H
#define RENDER_H

//#include "imgui.h"
//#include "imgui_impl_sdl.h"
//#include "imgui_impl_sdlrenderer.h"

// ================================ Rendering ==========================================
// Renders an individual tile to the screen, given its game coords and color.
void
RenderTile(int col, int row, const SDL_Color &color)
{
    SDL_Rect tileRect = {col * TILE_SIZE, row * TILE_SIZE, TILE_SIZE, TILE_SIZE};

    SDL_SetRenderDrawColor(GlobalRenderer, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(GlobalRenderer, &tileRect);
    SDL_SetRenderDrawColor(GlobalRenderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(GlobalRenderer, &tileRect);
}

// Renders a sprite to the screen, given its game coords and spritesheet.
void
RenderSprite(int col, int row, const SpriteSheet &sheet)
{
    SDL_Rect destination = {col * TILE_SIZE, 
                            row * TILE_SIZE, 
                            TILE_SIZE, TILE_SIZE};
    SDL_Rect source = {sheet.frame * sheet.size, sheet.track * sheet.size, sheet.size, sheet.size};

    SDL_RenderCopy(GlobalRenderer, sheet.texture.sdlTexture, &source, &destination);
}

// Renders a given texture at a pixel point.
// (Meant for debug text)
void
RenderText(const Texture &texture, int x, int y)
{
    SDL_Rect destination = {x, y, texture.width, texture.height};
    SDL_RenderCopy(GlobalRenderer, texture.sdlTexture, NULL, &destination);
}

// Renders a Health Bar
void
RenderHealthBar(int x, int y, int hp, int maxHp)
{
    SDL_Rect healthRect;
    for(int i = 0; i < maxHp; ++i)
    {
        SDL_Rect healthRect = {x + HEALTH_TICK_WIDTH * i, y, HEALTH_TICK_WIDTH, HEALTH_TICK_HEIGHT};
        if(i < hp)
        {
            SDL_SetRenderDrawColor(GlobalRenderer, healthBarColor.r, healthBarColor.g, healthBarColor.b, healthBarColor.a);
        }
        else
        {
            SDL_SetRenderDrawColor(GlobalRenderer, healthBarLostColor.r, healthBarLostColor.g, healthBarLostColor.b, healthBarLostColor.a);
        }
        SDL_RenderFillRect(GlobalRenderer, &healthRect);
        SDL_SetRenderDrawColor(GlobalRenderer, outlineColor.r, outlineColor.g, outlineColor.b, outlineColor.a);
        SDL_RenderDrawRect(GlobalRenderer, &healthRect);
    }
}

// Renders a Health Bar, with a damage amount taken out of it.
void
RenderHealthBarDamage(int x, int y, int hp, int maxHp, int Dmg)
{
    SDL_Rect healthRect;
    for(int i = 0; i < maxHp; ++i)
    {
        SDL_Rect healthRect = {x + HEALTH_TICK_WIDTH * i, y, HEALTH_TICK_WIDTH, HEALTH_TICK_HEIGHT};
        if(i < hp - Dmg)
        {
            SDL_SetRenderDrawColor(GlobalRenderer, healthBarColor.r, healthBarColor.g, healthBarColor.b, healthBarColor.a);
        }
        else if(i < hp)
        {
            SDL_SetRenderDrawColor(GlobalRenderer, healthBarLosingColor.r, healthBarLosingColor.g, healthBarLosingColor.b, healthBarLosingColor.a);
        }
        else
        {
            SDL_SetRenderDrawColor(GlobalRenderer, healthBarLostColor.r, healthBarLostColor.g, healthBarLostColor.b, healthBarLostColor.a);
        }
        SDL_RenderFillRect(GlobalRenderer, &healthRect);
        SDL_SetRenderDrawColor(GlobalRenderer, outlineColor.r, outlineColor.g, outlineColor.b, outlineColor.a);
        SDL_RenderDrawRect(GlobalRenderer, &healthRect);
    }
}


// Renders the scene from the given game state.
void
Render(const Tilemap &map, const Cursor &cursor, 
       const Menu &gameMenu, const Menu &unitMenu,
       const UnitInfo &unitInfo,
       const TileInfo &tileInfo,
       const CombatInfo &combatInfo)
{
    SDL_SetRenderDrawBlendMode(GlobalRenderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(GlobalRenderer, backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
    SDL_RenderClear(GlobalRenderer);

// ================================= render map tiles ============================================
    for(int col = cursor.viewportCol; col < cursor.viewportSize + cursor.viewportCol; ++col)
    {
        for(int row = cursor.viewportRow; row < cursor.viewportSize + cursor.viewportRow; ++row)
        {
            int screenCol = col - cursor.viewportCol;
            int screenRow = row - cursor.viewportRow;
            const Tile *tileToRender = &map.tiles[col][row];
            switch(tileToRender->type)
            {
                case(FLOOR):
                {
                    RenderTile(screenCol, screenRow, floorColor);
                } break;
                case(WALL):
                {
                    RenderTile(screenCol, screenRow, wallColor);
                } break;

                default:
                {} break;
            }

        }
    }

// ================================= render selected or targeted =====================================
    if(GlobalInterfaceState == SELECTED_OVER_GROUND ||
       GlobalInterfaceState == SELECTED_OVER_INACCESSIBLE ||
       GlobalInterfaceState == SELECTED_OVER_ALLY ||
       GlobalInterfaceState == SELECTED_OVER_ENEMY)
    {
        for(pair<int, int> cell : map.accessible)
        {
            if(cursor.WithinViewport(cell.first, cell.second))
            {
                RenderTile(cell.first - cursor.viewportCol, 
                           cell.second - cursor.viewportRow, 
                           moveColor);
            }
        }
    }

    if(GlobalInterfaceState == ENEMY_INFO)
    {
        for(pair<int, int> cell : map.accessible)
        {
            if(cursor.WithinViewport(cell.first, cell.second))
            {
                RenderTile(cell.first - cursor.viewportCol, 
                           cell.second - cursor.viewportRow, 
                           aiMoveColor);
            }
        }
    }

    if(GlobalInterfaceState == ATTACK_TARGETING_OVER_TARGET ||
       GlobalInterfaceState == ATTACK_TARGETING_OVER_UNTARGETABLE)
    {
        for(pair<int, int> cell : map.interactible)
        {
            if(cursor.WithinViewport(cell.first, cell.second))
            {
                RenderTile(cell.first - cursor.viewportCol, 
                           cell.second - cursor.viewportRow, 
                           attackColor);
            }
        }
    }

    if(GlobalInterfaceState == HEALING_TARGETING_OVER_TARGET ||
       GlobalInterfaceState == HEALING_TARGETING_OVER_UNTARGETABLE)
    {
        for(pair<int, int> cell : map.interactible)
        {
            if(cursor.WithinViewport(cell.first, cell.second))
            {
                RenderTile(cell.first - cursor.viewportCol, 
                           cell.second - cursor.viewportRow, 
                           healColor);
            }
        }
    }

// ================================ ai visualization  =============================
    if(GlobalAIState == SELECTED)
    {
        for(pair<int, int> cell : map.accessible)
        {
            if(cursor.WithinViewport(cell.first, cell.second))
            {
                RenderTile(cell.first - cursor.viewportCol, 
                           cell.second - cursor.viewportRow, 
                           aiMoveColor);
            }
        }
    }

// ================================= render sprites ================================================
    for(int col = cursor.viewportCol; col < cursor.viewportSize + cursor.viewportCol; ++col)
    {
        for(int row = cursor.viewportRow; row < cursor.viewportSize + cursor.viewportRow; ++row)
        {
            int screenCol = col - cursor.viewportCol;
            int screenRow = row - cursor.viewportRow;
            const Tile *tileToRender = &map.tiles[col][row];
            if(tileToRender->occupied)
            {
                if(tileToRender->occupant->isExhausted)
                {
                    SDL_SetTextureColorMod(tileToRender->occupant->sheet.texture.sdlTexture, 50, 0, 50);
                }
                else
                {
                    SDL_SetTextureColorMod(tileToRender->occupant->sheet.texture.sdlTexture, 255, 255, 255);
                }
                RenderSprite(screenCol, screenRow, tileToRender->occupant->sheet);
            }
        }
    }


// ================================= render cursor ================================================
    RenderSprite(cursor.col - cursor.viewportCol, cursor.row - cursor.viewportRow, cursor.sheet);


// ================================= render ui elements ===========================================
    // Tile Info
    if(GlobalInterfaceState == NEUTRAL_OVER_ENEMY || 
       GlobalInterfaceState == NEUTRAL_OVER_UNIT ||
       GlobalInterfaceState == NEUTRAL_OVER_DEACTIVATED_UNIT ||
       GlobalInterfaceState == SELECTED_OVER_ALLY ||
       GlobalInterfaceState == SELECTED_OVER_ENEMY ||
       GlobalInterfaceState == ATTACK_TARGETING_OVER_TARGET ||
       GlobalInterfaceState == HEALING_TARGETING_OVER_TARGET ||
       GlobalInterfaceState == GAME_MENU_ROOT ||
       GlobalInterfaceState == UNIT_MENU_ROOT)
    {
        for(int i = 0; i < tileInfo.rows; ++i)
        {
            SDL_Rect infoRect = {TILE_SIZE * cursor.viewportSize, i * MENU_ROW_HEIGHT, MENU_WIDTH, MENU_ROW_HEIGHT};
            SDL_SetRenderDrawColor(GlobalRenderer, uiAltColor.r, uiAltColor.g, uiAltColor.b, uiAltColor.a);
            SDL_RenderFillRect(GlobalRenderer, &infoRect);
            SDL_SetRenderDrawColor(GlobalRenderer, outlineColor.r, outlineColor.g, outlineColor.b, outlineColor.a);
            SDL_RenderDrawRect(GlobalRenderer, &infoRect);

            RenderText(tileInfo.infoTextTextures[i], infoRect.x, infoRect.y);
        }
        RenderHealthBar(TILE_SIZE * cursor.viewportSize, 2 * MENU_ROW_HEIGHT, tileInfo.hp, tileInfo.maxHp);
                                                      // 2 is for alignment
    }

    // Unit Info
    if(GlobalInterfaceState == UNIT_INFO ||
       GlobalInterfaceState == ENEMY_INFO)
    {
        for(int i = 0; i < unitInfo.rows; ++i)
        {
            SDL_Rect infoRect = {TILE_SIZE * cursor.viewportSize, i * MENU_ROW_HEIGHT, MENU_WIDTH, MENU_ROW_HEIGHT};

            if(GlobalInterfaceState == UNIT_INFO)
            {
                SDL_SetRenderDrawColor(GlobalRenderer, uiColor.r, uiColor.g, uiColor.b, uiColor.a);
            }
            else
            {
                SDL_SetRenderDrawColor(GlobalRenderer, enemyColor.r, enemyColor.g, enemyColor.b, enemyColor.a);
            }
            SDL_RenderFillRect(GlobalRenderer, &infoRect);
            SDL_SetRenderDrawColor(GlobalRenderer, outlineColor.r, outlineColor.g, outlineColor.b, outlineColor.a);
            SDL_RenderDrawRect(GlobalRenderer, &infoRect);

            RenderText(unitInfo.infoTextTextures[i], infoRect.x, infoRect.y);
        }
    }
    
    // Combat Preview
    if(GlobalInterfaceState == PREVIEW_ATTACK ||
       GlobalInterfaceState == PREVIEW_HEALING)
    {
        for(int i = 0; i < combatInfo.rows; ++i)
        {
            SDL_Rect sourceRect = {TILE_SIZE * cursor.viewportSize, i * MENU_ROW_HEIGHT, MENU_WIDTH, MENU_ROW_HEIGHT};

            SDL_Rect targetRect = {TILE_SIZE * cursor.viewportSize, i * MENU_ROW_HEIGHT + combatInfo.rows * sourceRect.h, MENU_WIDTH, MENU_ROW_HEIGHT};

            SDL_SetRenderDrawColor(GlobalRenderer, uiColor.r, uiColor.g, uiColor.b, uiColor.a);
            SDL_RenderFillRect(GlobalRenderer, &sourceRect);
            SDL_SetRenderDrawColor(GlobalRenderer, enemyColor.r, enemyColor.g, enemyColor.b, enemyColor.a);
            SDL_RenderFillRect(GlobalRenderer, &targetRect);
            SDL_SetRenderDrawColor(GlobalRenderer, outlineColor.r, outlineColor.g, outlineColor.b, outlineColor.a);
            SDL_RenderDrawRect(GlobalRenderer, &sourceRect);
            SDL_RenderDrawRect(GlobalRenderer, &targetRect);

            RenderText(combatInfo.sourceTextTextures[i], sourceRect.x, sourceRect.y);
            RenderText(combatInfo.targetTextTextures[i], targetRect.x, targetRect.y);
        }
        RenderHealthBarDamage(TILE_SIZE * cursor.viewportSize, MENU_ROW_HEIGHT, combatInfo.unitHp, combatInfo.unitMaxHp, combatInfo.enemyDamage);
        RenderHealthBarDamage(TILE_SIZE * cursor.viewportSize, 5 * MENU_ROW_HEIGHT, combatInfo.enemyHp, combatInfo.enemyMaxHp, combatInfo.unitDamage);
                                                      // 5 is for alignment
    }


// ==================================== menus =====================================================
    // Game Menu
    if(GlobalInterfaceState == GAME_MENU_ROOT)
    {
        for(int i = 0; i < gameMenu.rows; ++i)
        {
            SDL_Rect menuRect = {TILE_SIZE * cursor.viewportSize, i * MENU_ROW_HEIGHT, MENU_WIDTH, MENU_ROW_HEIGHT};

            SDL_SetRenderDrawColor(GlobalRenderer, uiColor.r, uiColor.g, uiColor.b, uiColor.a);
            SDL_RenderFillRect(GlobalRenderer, &menuRect);
            SDL_SetRenderDrawColor(GlobalRenderer, outlineColor.r, outlineColor.g, outlineColor.b, outlineColor.a);
            SDL_RenderDrawRect(GlobalRenderer, &menuRect);

            RenderText(gameMenu.optionTextTextures[i], menuRect.x, menuRect.y);
        }

        SDL_Rect menuSelectorRect = {TILE_SIZE * cursor.viewportSize, MENU_ROW_HEIGHT * gameMenu.current, MENU_WIDTH, MENU_ROW_HEIGHT};
        SDL_SetRenderDrawColor(GlobalRenderer, uiSelectorColor.r, uiSelectorColor.g, uiSelectorColor.b, uiSelectorColor.a);
        SDL_RenderFillRect(GlobalRenderer, &menuSelectorRect);
        SDL_SetRenderDrawColor(GlobalRenderer, outlineColor.r, outlineColor.g, outlineColor.b, outlineColor.a);
        SDL_RenderDrawRect(GlobalRenderer, &menuSelectorRect);
    }

    // Unit Menu
    if(GlobalInterfaceState == UNIT_MENU_ROOT)
    {
        for(int i = 0; i < unitMenu.rows; ++i)
        {
            SDL_Rect menuRect = {TILE_SIZE * cursor.viewportSize, i * MENU_ROW_HEIGHT, MENU_WIDTH, MENU_ROW_HEIGHT};

            SDL_SetRenderDrawColor(GlobalRenderer, uiColor.r, uiColor.g, uiColor.b, uiColor.a);
            SDL_RenderFillRect(GlobalRenderer, &menuRect);
            SDL_SetRenderDrawColor(GlobalRenderer, outlineColor.r, outlineColor.g, outlineColor.b, outlineColor.a);
            SDL_RenderDrawRect(GlobalRenderer, &menuRect);

            RenderText(unitMenu.optionTextTextures[i], menuRect.x, menuRect.y);
        }

        SDL_Rect menuSelectorRect = {TILE_SIZE * cursor.viewportSize, MENU_ROW_HEIGHT * unitMenu.current, MENU_WIDTH, MENU_ROW_HEIGHT};
        SDL_SetRenderDrawColor(GlobalRenderer, uiSelectorColor.r, uiSelectorColor.g, uiSelectorColor.b, uiSelectorColor.a);
        SDL_RenderFillRect(GlobalRenderer, &menuSelectorRect);
        SDL_SetRenderDrawColor(GlobalRenderer, uiTextColor.r, uiTextColor.g, uiTextColor.b, uiTextColor.a);
        SDL_RenderDrawRect(GlobalRenderer, &menuSelectorRect);
    }

    // Dev Gui
    if(GlobalGuiMode)
    {
        //ImGui_ImplSDLRenderer_NewFrame();
        //ImGui_ImplSDL2_NewFrame();
        //ImGui::NewFrame();
        //{
            //static float f = 0.0f;
            //static int counter = 0;

            //ImGui::Begin("Hello, world!");

            //ImGui::Text("This is some useful text.");

            //ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
            //if (ImGui::Button("Button"))
                //counter++;
            //ImGui::SameLine();
            //ImGui::Text("counter = %d", counter);

            //ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            //ImGui::End();
        //}
        //ImGui::Render();
        //ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
    }


    SDL_RenderPresent(GlobalRenderer);
}

#endif
