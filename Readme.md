Puzzle-A-Day Solver
===================

A command line tool for solving the Puzzle-A-Day and providing interesting statistics about possible solutions.
The solver can be restricted to only find solutions with pieces faced up/down, have a custom board layout and custom pieces altogether. It can also be instructed to find invalid solutions such as two months (Jan Feb), two days (3 14) or non existant days (Feb 31). This tool was inspired by the work done at [https://www.noreply.blog/a-puzzle-a-day/](https://www.noreply.blog/a-puzzle-a-day/) and expanded to provide an interactive experience.

![](https://ksr-ugc.imgix.net/assets/031/372/109/d6494e4d6e7107530170c2e44689d292_original.jpg?ixlib=rb-4.0.2&crop=faces&w=1552&h=873&fit=crop&v=1605108388&auto=format&frame=1&q=92&s=b14516a5c0d9e1976245ed181eed6129)

The standard puzzle formulation from this Kickstarter [A-Puzzle-A-Day](https://www.kickstarter.com/projects/dragonfjord/a-puzzle-a-day) consists of a board with Months and Days and the goal is to fit the 8 pieces onto the board with the two remaining squares showing the current date. Naturally some days have more than one solution, some can be solved with all pieces facing up, some combinations however are impossible. This tool helps to uncover it all.


## Usage

```bash
$ # Building using Make
$ make
$ 
$ # Find solutions for 2 August with all tiles facing up
$ ./pad mar 14 faceup
```

>The build requires OpenMP installed

### Example Run
![Asciicast Demo](./demo/demo.svg)

## The Game
### Standard Board
```
**************************
*Jan Feb Mar Apr May Jun *
*Jul Aug Sep Oct Nov Dec *****
*  1   2   3   4   5   6   7 *
*  8   9  10  11  12  13  14 *
* 15  16  17  18  19  20  21 *
* 22  23  24  25  26  27  28 *
* 29  30  31 *****************
**************
```

### Standard Pieces

1. Symetric + 2 rotations, 48 valid positions
    ```
    # # #
    # # #
    ```
2. Symetric + 4 rotations, 80 valid positions
    ```
    #
    #
    # # #
    ```
3. Asymetric + 2 rotations, 82 valid positions
    ```
        #
    # # #
    #
    ```
4. Symetric + 4 rotations, 96 valid positions
    ```
    #   #
    # # #
    ```
5. Asymetric + 4 rotations, 151 valid positions
    ```
    #
    # # # #
    ```
6. Asymetric + 4 rotations, 154 valid positions
    ```
        # 
    # # # #
    ```

7. Asymetric + 4 rotations, 154 valid positions
    ```
        # #
    # # #
    ```
8. Asymetric + 8 rotations, 196 valid positions

    ```
    # #
    # # # 
    ```



# Todo
 - [ ] Add easier support for Multiple Piece types
 - [ ] Support for alternate board layouts
 - [ ] Support for Days of week
 - [ ] Include tool for validating board and piece choice compatibility
