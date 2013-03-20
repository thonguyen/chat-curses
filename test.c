/*
 * =====================================================================================
 *
 *       Filename:  test.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/19/2013 01:53:59 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */


/*
 * =====================================================================================
 *
 *       Filename:  interface.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/15/2013 11:16:38 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include<errno.h>
#include<stdlib.h>
#include<string.h>
#include<ncurses.h>



int main(){
    initscr();
    cbreak();
    start_color();
    use_default_colors();
    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_WHITE);
    init_pair(3, COLOR_CYAN, COLOR_BLACK);
        wattron(COLOR_PAIR(1));
        wprintw(stdscr, "xxxxxxxxxxxxxxxxxx");
        wattroff(COLOR_PAIR(1));
    getch();
    return 0;
}
