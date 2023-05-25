#ifndef protocol_printing
#define protocol_printing

/* Declare this in a header file somewhere. */
static uint16 *vid_mem = (uint16 *)0xB8000;
static uint8 *vid_mem2 = (uint8 *)0xB8000;

#ifndef FAMP_colors
#include "colors.h"
#endif

const uint8 *color_names[] = {
    "black", "@bblue@w", "@ggreen@w", "@ccyan@w",
    "@rred@w", "@mmagenta@w", "@brbrown@w", "@lglight grey@w",
    "@dgdark grey@w", "@lblight blue@w", "@lglime green@w",
    "@lclight cyan@w", "@lrlight red@w", "@lmlight magenta@w",
    "@yyellow@w", "white"
};

/* The "color theme" that is presented throughout the program. */
static uint8 color;

/* The default color will be whatever value `color` has originally when the screen is initialized. */
static uint8 default_color;

/* Store the character going "off screen". `5000` is hard coded for now.
 * TODO: Figure out how to make `row_data` flexible.
 * */
static uint16 row_data[5000];
static uint8 row_data_index = 0;

void scroll_down()
{
    /* Get all of the characters that are on the screen. */
    uint16 vid_mem_2[rows];
    for(uint8 i = 0; i < rows - 1; i++)
        vid_mem_2[i] = vid_mem[0 + ((i+1) * 80)];
    
    /* Store whatever value that is on the first line, and increment `row_data_index`. */
    row_data[row_data_index] = vid_mem[0];
    row_data_index++;

    /* Since there was an `enter` key pressed on the last line, we'll go back one and put a empty character. */
    c_info.pos_y--;
    put_char(' ');
    c_info.pos_x--;

    /* Copy over the buffer accordingly to make it look like we "scrolled". */
    for(uint8 i = 0; i < rows - 1; i++)
    {
        vid_mem[0 + (i * 80)] = get_text_value(vid_mem_2[i] & 0xFF, (color >> 4) & 0x0F, color & 0x0F);
    }
}

void scroll_up()
{
    /* Get all of the characters that are currently on the screen. */
    uint16 vid_mem_2[rows];
    for(uint8 i = 0; i < rows; i++)
        vid_mem_2[i] = vid_mem[0 + ((i) * 80)];

    /* `ly`(last y) is to store the last Y position of the cursor.*/
    uint16 ly = c_info.pos_y;

    /* Set the y position to 1 and rewrite values to the buffer accordingly. */
    c_info.pos_y = 1;
    for(uint8 i = 0; i < rows - 1; i++)
    {
        vid_mem[0 + ((c_info.pos_y) * 80)] = get_text_value(vid_mem_2[i] & 0xFF, (color >> 4) & 0x0F, color & 0x0F);
        c_info.pos_y++;
    }
    
    /* Set the Y positin to 0 to copy the correct buffer from `row_data` to the correct place. */
    c_info.pos_y = 0;

    /* Make sure the spot is empty. Set the X position to "override" the empty space. */
    put_char(' ');
    c_info.pos_x = 0;

    /* Put the character in its place. Decrement `row_data_index`. */
    put_char(row_data[row_data_index - 1] & 0xFF);
    row_data_index--;

    /* Reset the Y position, aswell as the X position and update the cursor accordingly. */
    c_info.pos_y = ly;
    c_info.pos_x = last_cursor_x[last_cursor_x_index - 1];
    last_cursor_x_index--;
    update_cursor_pos();
}

void put_char(uint8 character)
{
    static uint8 t = 0;
    switch(character)
    {
        case '\n':
        {
            c_info.pos_y++;
            last_cursor_x[last_cursor_x_index] = c_info.pos_x;
            last_cursor_x_index++;
            c_info.pos_x = 0;

            /* Scroll if the y position is >= 25. */
            if(c_info.pos_y >= rows)
                scroll_down();

            goto end;
        }
        case '\t':
        {
            c_info.pos_x+=4;
            goto end;
        }
        case 0x0E:
        {
            if(c_info.pos_x >= 1)
            {
                c_info.pos_x--;
                vid_mem[c_info.pos_x + (c_info.pos_y * 80)] = (color << 8) | (0xFF & ' ');
                goto end;
            }

            if(row_data_index > 0)
            {
                scroll_up();
                goto end;
            }

            if(c_info.pos_y > 0)
            {
                /* TODO: Make it to where the characters that are "off" the screen
                 * get saves, and when we backspace enough they reappear and the
                 * characters already on the screen get shifted down.
                 * Some of this will have to be done in `scroll`(saving the character going "off" the screen).
                 */
                c_info.pos_y--;
                last_cursor_x_index--;
                c_info.pos_x = last_cursor_x[last_cursor_x_index];
                goto end;
            }
            return;
        }
        /* Do nothing if the above does not run. */
        default: break;
    }
    vid_mem[c_info.pos_x + (c_info.pos_y * 80)] =  (color << 8) | (0xFF & character);
    c_info.pos_x++;

    end:
    update_cursor_pos();
}

void print(const uint8 *str, uint8 endc)
{
    lsize i = 0;

    while(str[i] != '\0')
    {
        /* Check the current character to see if it is any of the given
         * "special" characters.
         */
        redo:
        switch(str[i])
        {
            case '\n':
            {
                c_info.pos_y++;
                last_cursor_x[last_cursor_x_index] = c_info.pos_x;
                last_cursor_x_index++;
                c_info.pos_x = 0;
                i++;
                goto redo;
            }
            case '\t':c_info.pos_x += 4;i++;goto redo;
            case '@':
            {
                i++;
                
                switch(str[i])
                {
                    case 'l':
                    {
                        if(str[i+1] == 'g') { i+=2; color = light_grey; goto redo; }
                        if(str[i+1] == 'b') { i+=2; color = light_blue; goto redo; }
                        if(str[i+1] == 'G') { i+=2; color = lime_green; goto redo; }
                        if(str[i+1] == 'c') { i+=2; color = light_cyan; goto redo; }
                        if(str[i+1] == 'r') { i+=2; color = light_red; goto redo; }
                        if(str[i+1] == 'm') { i+=2; color = light_magenta; goto redo; }

                        /* If it is not of the above just skip and make sure the color is the default color. */
                        i++;
                        color = default_color;
                        goto redo;
                    }
                    case 'd':
                    {
                        if(str[i+1] == 'g') { i+=2; color = dark_grey; goto redo; }

                        i++;
                        color = default_color;
                        goto redo;
                    }
                    case 'y':
                    {
                        color = yellow;
                        i++;
                        goto redo;
                    }
                    case ':':
                    {
                        c_info.pos_x = 0;
                        c_info.pos_y = 0;
                        i++;
                        goto redo;
                    }
                    case 'w':
                    {
                        color = white;
                        i++;
                        goto redo;
                    }
                    case 'b':
                    {
                        if(str[i+1] == 'r') { i++; color = brown; }
                        else color = blue;
                        i++;
                        goto redo;
                    }
                    case 'B':
                    {
                        color = black;
                        i++;
                        goto redo;
                    }
                    case 'r':
                    {
                        if(str[i+1] == 'e') { i++;color = default_color; }
                        else color = red;
                        i++;
                        goto redo;
                    }
                    case 'c':
                    {
                        color = cyan;
                        i++;
                        goto redo;
                    }
                    case 'm':
                    {
                        color = magenta;
                        i++;
                        goto redo;
                    }
                    case 'g':
                    {
                        color = green;
                        i++;
                        goto redo;
                    }
                    default: i++;goto redo;
                }
            }
            case '\0':goto end;
            default: break;
        }

        /* Write character to according index in the video memory buffer. */
        vid_mem[c_info.pos_x + (c_info.pos_y * 80)] = (color << 8) | (0xFF & str[i]);

        /* Increment index/cursor x position. */
        i++;
        c_info.pos_x++;

        /* Check if the x position surpasses or is directly equal to
         * the width of the screen. If it is, add newline.
         */
        if(c_info.pos_x >= 80)
        {
            c_info.pos_x = 0;
            c_info.pos_y++;
        }

        update_cursor_pos();
    }

    end:
    if(endc != 0 || endc != '\0') put_char(endc);
    return;
}

char * itoa( int value, char * str, int base )
{
    char * rc;
    char * ptr;
    char * low;
    // Check for supported base.
    if ( base < 2 || base > 36 )
    {
        *str = '\0';
        return str;
    }
    rc = ptr = str;
    // Set '-' for negative decimals.
    if ( value < 0 && base == 10 )
    {
        *ptr++ = '-';
    }
    // Remember where the numbers start.
    low = ptr;
    // The actual conversion.
    do
    {
        // Modulo is negative for negative value. This trick makes abs() unnecessary.
        *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz"[35 + value % base];
        value /= base;
    } while ( value );
    // Terminating the string.
    *ptr-- = '\0';
    // Invert the numbers.
    while ( low < ptr )
    {
        char tmp = *low;
        *low++ = *ptr;
        *ptr-- = tmp;
    }
    return rc;
}

/* Create a temporary buffer at the memory address `0x1000`. */
static lsize amnt_of_elements_stored_in_1000H_buffer = 0;
uint8 *init_new_char_array(uint8 *arr, lsize elements, uint8 *string)
{
    uint8 *arrPtr;
    amnt_of_elements_stored_in_1000H_buffer = elements;

    if(arr == NULL) arrPtr = (uint8 *)0x1000;
    else arrPtr = arr;

    memsetb(arr, 0, elements);

    /* -1 for `\0` at end. */
    if(string != NULL)
        for(uint16 i = 0; i < elements - 1; i++)
            arrPtr[i] = string[i];
    
    arrPtr[elements - 1] = '\0';
    //addr += elements;
    
    return arrPtr;
}

/* Clear out the buffer stored in `0x1000`. */
void clear_char_array()
{
    uint8 *arrPtr = (uint8 *)0x1000;
    
    /* Clear out the buffer, and set the amount of elements stored in the buffer to zero. */
    memsetb(arrPtr, 0, amnt_of_elements_stored_in_1000H_buffer);
    arrPtr = NULL;
    amnt_of_elements_stored_in_1000H_buffer = 0;
}

bool strcmp(uint8 *string1, uint8 *string2)
{
    uint16 length1 = 0;
    uint16 length2 = 0;

    while(string1[length1] != '\0') length1++;
    while(string2[length2] != '\0') length2++;

    if(length1 > length2 || length1 < length2) return false;

    length1 = length2 = 0;
    while(string1[length1] != '\0')
    {
        if(string1[length1] != string2[length1]) return false;
        length1++;
    }

    return true;
}

void clear()
{
    uint16 i = 0;
    while(i < rows*cols)
    {
        memset((uint16 *)0xB8000 + (i * cols), get_text_value(' ', 0x00, 0x0F), cols);
        i++;
    }
    zero_out_cursor_pos();
}

uint8 get_raw_color_value()
{
    redo_new_color:
    get_long_input(NULL);

    if(strcmp(input_buffer, init_new_char_array(NULL, 5, "back")) == true) return 'b';

    if(strcmp(input_buffer, init_new_char_array(NULL, 6, "black")) == true)            return black;
    if(strcmp(input_buffer, init_new_char_array(NULL, 5, "blue")) == true)             return blue;
    if(strcmp(input_buffer, init_new_char_array(NULL, 6, "green")) == true)            return green;
    if(strcmp(input_buffer, init_new_char_array(NULL, 5, "cyan")) == true)             return cyan;
    if(strcmp(input_buffer, init_new_char_array(NULL, 4, "red")) == true)              return red;
    if(strcmp(input_buffer, init_new_char_array(NULL, 8, "magenta")) == true)          return magenta;
    if(strcmp(input_buffer, init_new_char_array(NULL, 6, "brown")) == true)            return brown;
    if(strcmp(input_buffer, init_new_char_array(NULL, 11, "light grey")) == true)      return light_grey;
    if(strcmp(input_buffer, init_new_char_array(NULL, 10, "dark grey")) == true)       return dark_grey;
    if(strcmp(input_buffer, init_new_char_array(NULL, 11, "light blue")) == true)      return light_blue;
    if(strcmp(input_buffer, init_new_char_array(NULL, 11, "lime green")) == true)      return lime_green;
    if(strcmp(input_buffer, init_new_char_array(NULL, 11, "light cyan")) == true)      return light_cyan;
    if(strcmp(input_buffer, init_new_char_array(NULL, 10, "light red")) == true)       return light_red;
    if(strcmp(input_buffer, init_new_char_array(NULL, 14, "light magenta")) == true)   return light_magenta;
    if(strcmp(input_buffer, init_new_char_array(NULL, 7, "yellow")) == true)           return yellow;
    if(strcmp(input_buffer, init_new_char_array(NULL, 6, "white")) == true)            return white;

    clear();
    print("\nOops.. I don't thinks `@y", 0);
    print(input_buffer, '\0');
    print("` @wis a valid color. Try again :)", '\n');

    /* Reprint for convenience. */
    print("For reference, here is a list of all the available colors!(P.S you have to type the numbers out in lowercase)\n\t1. Black\n\t2. @bBlue@w\n\t3. @gGreen@w\n\t4. @cCyan@w\n\t5. @rRed@w\n\t6. @mMagenta@w\n\t7. @bBrown@w\n\t8. @lgLight Grey@w\n\t9. @dgDark Grey@w\n\t10. @lbLight Blue@w\n\t11. @lgLime Green@w\n\t12. @lcLight Cyan@w\n\t13. @lrLight Red@w\n\t14. @lmLight Magenta@w\n\t15. @yYellow@w\n\t16. White@w\n> ", 0);
    
    goto redo_new_color;
}

void clear_screen(uint8 *function_name, uint8 bgcolor, uint8 fgcolor)
{
    if(get_text_attribute(bgcolor, fgcolor) == 0 && oinfo->in_production == true)
    {
        begin_reset_colors:

        clear();
        
        /* Go ahead and setup a temporary color/default color so users can see the messages being printed. */
        color = get_text_attribute(0x00, 0x0F);
        default_color = color;

        print("You might have made a @rmistake@w in the function `", 0);
        print(function_name, 0);
        print("`!\n@wYou set your foreground and your background to \nthe same color. Your foreground is ", 0);
        print(color_names[fgcolor], 0);
        print(" and you background color is ", 0);
        print(color_names[bgcolor], '\n');
        
        redo:
        get_char("Which would you like to change?\n\t1. Foreground\n\t2. Background\n\t3. It wasn't a mistake\n> ", true);

        clear();
        
        /* Print this before we continue onward. */
        print("For reference, here is a list of all the available colors!\n(P.S you have to type the numbers out in lowercase. Type \"back\" to go back.)\n\t1. Black\n\t2. @bBlue@w\n\t3. @gGreen@w\n\t4. @cCyan@w\n\t5. @rRed@w\n\t6. @mMagenta@w\n\t7. @brBrown@w\n\t8. @lgLight Grey@w\n\t9. @dgDark Grey@w\n\t10. @lbLight Blue@w\n\t11. @lGLime Green@w\n\t12. @lcLight Cyan@w\n\t13. @lrLight Red@w\n\t14. @lmLight Magenta@w\n\t15. @yYellow@w\n\t16. White@w\n> ", 0);

        if(user_input == '1') goto fg;
        if(user_input == '2') goto bg;
        if(user_input == '3') goto endo;

        fg:

        /* Get the new foreground color. */
        uint8 nfgcolor = get_raw_color_value();
        if(nfgcolor == 'b') { user_input = 0; goto begin_reset_colors; }
        fgcolor = nfgcolor;

        /* Update the color. */
        color = get_text_attribute(bgcolor, fgcolor);

        goto check_colors;

        bg:

        /* Get the new background color. */
        uint8 nbgcolor = get_raw_color_value();
        if(nbgcolor == 'b') { user_input = 0; goto begin_reset_colors; }
        bgcolor = nbgcolor;

        /* Update the color. */
        color = get_text_attribute(bgcolor, fgcolor);

        check_colors:
        clear_char_array();
        
        /* Make sure that the background doesn't match the foreground still. We only care about the least significant 4-bits here. */
        if(fgcolor == bgcolor || (fgcolor & 0x0F) == bgcolor || (fgcolor & 0x0F) == (bgcolor & 0x0F))
        {
            color = default_color;

            /* Set `user_input` to zero so we are ready to get new input. Also clear the screen of all the junk. */
            user_input = 0;
            clear();

            /* Ask the user if they want FAMP to go ahead and just initialize the background color to be white. */
            get_char("Do you want FAMP to go ahead and initialize the background to be white?\n[y/n] > ", true);
            
            put_char(user_input);
            put_char('\n');

            if(user_input == 'y') color = get_text_attribute(white, black);//bgcolor = white;
            else goto begin_reset_colors;
        }
    } else {
        endo:
        color = get_text_attribute(bgcolor, fgcolor);
    }

    uint16 i = 0;
    while(i < rows*cols)
    {
        memset((uint16 *)0xB8000 + (i * cols), get_text_value(' ', (color >> 4) & 0x0F, color & 0x0F), cols);
        i++;
    }
    zero_out_cursor_pos();

    default_color = color;
}

#endif