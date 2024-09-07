# QPDFViewer

A lightweight PDF viewer made using Qt5 and Poppler.

# Noteworthy Features
- Clean user interface
- Tab support
- Navigation Bar
- Text selection
- Searching
- Printing

# Build notes
This project has been built with VS2022, vcpkg, and Windows in mind, although ports to Linux and the Mac should be trivial.

`poppler-cpp` also does not support reading the page numbers of the table of content items by default, so I had to patch it into Poppler. You can either replace `private-namespace.patch` in the `vcpkg\ports\poppler` folder with the one provided in this repository, or build Poppler by yourself using this [fork](https://github.com/DavidB420/poppler-pagenum).
