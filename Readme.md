Puzzle-A-Day Solver
===================
[![Buy me a Beer](https://img.shields.io/badge/Buy%20me%20a%20Beer-%245-orange)](https://www.buymeacoffee.com/mandula)


A command line tool for solving the Puzzle-A-Day and providing interesting statistics about possible solutions.
The solver can be restricted to only find solutions with pieces faced up/down, have a custom board layout and custom pieces altogether. It can also be instructed to find invalid solutions such as two months (Jan Feb), two days (3 14) or non existant days (Feb 31). This tool was inspired by the work done at [https://www.noreply.blog/a-puzzle-a-day/](https://www.noreply.blog/a-puzzle-a-day/) and expanded to provide an interactive experience.

![](https://ksr-ugc.imgix.net/assets/031/372/109/d6494e4d6e7107530170c2e44689d292_original.jpg?ixlib=rb-4.0.2&crop=faces&w=1552&h=873&fit=crop&v=1605108388&auto=format&frame=1&q=92&s=b14516a5c0d9e1976245ed181eed6129)

The standard puzzle formulation from this Kickstarter [A-Puzzle-A-Day](https://www.kickstarter.com/projects/dragonfjord/a-puzzle-a-day) consists of a board with Months and Days and the goal is to fit the 8 pieces onto the board with the two remaining squares showing the current date. Naturally some days have more than one solution, some can be solved with all pieces facing up, some combinations however are impossible. This tool helps to uncover it all.


## Basic Usage

```bash
$ # Building using Make
$ make
$ 
$ # Find solutions for 14 March with all tiles facing down
$ ./pad mar 14 facedown
```

>The build requires OpenMP installed

### Example Run
![Asciicast Demo](./demo/demo.svg)

## Advanced Features

1. Finding solutions for alternative board layouts (Including Weekday Positions, Blank 8x8)
  One can change to the extended Weeek-Day puzzle by specifying 3 target positons (Day, Month, WeekDay)
  
```bash
$ # Find solutions for 14 March, Friday with all tiles facing up
$ ./pad mar 14 fri faceup

$ # Find solutions for Friday, Saturday, Sunday
$ ./pad sat sun fri
```
<img src="./demo/weekdays.png"  width=50%>
  
2. Finding solutions for pre-placed pieces
  One can fix a piece position and restrict the solutions that match it

```bash
$ # Find solutions for 14 March, With Piece 7 placed at (4,1), rotated 3x and flipped
$ ./pad mar 14 7,4,1,3,1
```

<img src="./demo/placement.png"  width=50%>


3. Using alternative board pieces (T-shape, Face-Up optimized)
  Alternative board pieces can be selected for alternative puzzle solutions
```bash
$ # Find solutions for 14 March, Using the T piece puzzle version
$ ./pad mar 14 t
$ # Find solutions for 14 March, Using the FaceUp optimized puzzle version
$ ./pad mar 14 fu8
```
<img src="./demo/fu8.png"  width=50%>

4. Loading custom board pices from a text file
  Alternative board pieces can be specified using a text files
```bash
$ # Find solutions for 14 March, Facing up, using cusom pieces defined in the `pieces.txt` file
$ ./pad mar 14 c=pieces.txt faceup
```

<img src="./demo/custom.png"  width=100%>

5. Disable printing, only show solution count
```bash
$ # Only print number of faceup solutions for 14 March, not the actual placement
$ ./pad mar 14 faceup noprint
```
<img src="./demo/noprint.png"  width=50%>


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

### Week Days Board
```
**************************
*Jan Feb Mar Apr May Jun *
*Jul Aug Sep Oct Nov Dec *****
*  1   2   3   4   5   6   7 *
*  8   9  10  11  12  13  14 *
* 15  16  17  18  19  20  21 *
* 22  23  24  25  26  27  28 *
* 29  30  31 Mon Tue Wed Thu *
**************** Fri Sat Sun *
               ***************
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
8. Asymetric + 4 rotations, 196 valid positions

    ```
    # #
    # # # 
    ```



### WeekDay Pieces

1. Symetric + 2 rotations, 55 valid positions
    ``` 
    # # # #
    ```
2. Symetric + 4 rotations, 100 valid positions
    ```
    #
    #
    # # #
    ```
3. Asymetric + 2 rotations, 102 valid positions
    ```
        #
    # # #
    #
    ```
4. Symetric + 4 rotations, 102 valid positions
    ```
        #
    # # #
        #
    ```
5. Symetric + 4 rotations, 118 valid positions
    ```
    #   #
    # # #
    ```
6. Asymetric + 2 rotations, 122 valid positions
    ```
    # #
      # # 
    ```
7. Asymetric + 4 rotations, 191 valid positions
    ```
    #
    # # # #
    ```
8. Asymetric + 4 rotations, 194 valid positions
    ``` 
    # # #
        # #
    ```
9. Asymetric + 4 rotations, 240 valid positions
    ```
    # #
    # # #
10. Asymetric + 4 rotations, 242 valid positions
    ```
    #
    # # #
    ```

# Todo
 - [x] Add easier support for Multiple Piece types
 - [x] Support for alternate board layouts
 - [x] Support for Days of week
 - [ ] Include tool for validating board and piece choice compatibility
