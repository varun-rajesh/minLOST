Edit the [config.ini](config.ini) file with any parameter modifications you would like to make.

Run `make` to generate the necessary files and build the C program. The output should be called `star_tracker(.exe)`.

Using [LOST](https://github.com/UWCubeSat/lost), generate sample images with the same parameters as what was configured in the [config.ini](config.ini) file. Convert those png images to binary files using the `python png_to_rgb.py <png file>`.

You can run the star tracker by calling `./star_tracker <bin file>`. It should print out the associated attitude with that specific star image, if enough stars have been identified.