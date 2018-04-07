# detect-cars
Car detection and speed estimation on my street using OpenCV, with a Logitech C920.

Uses background subtraction by taking a mean image of the area and measuring a large car "blob" as it drives by.

usage:
```bash
  $ cmake .
  $ make
  $ ./detect-cars
```
