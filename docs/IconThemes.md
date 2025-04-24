How to use your own icons
==================

Create a directory inside the program directory
> `./themes/icons/My Icons Theme 1`


There is a full set of custom icons in directory 
> `./themes/icons/Hand Drawn 1`

Documentation: [icons.pdf](https://github.com/NgzQ3Ee5/DoffenSSHTunnel/blob/main/docs/misc/icons.pdf)

The icons you provide overrides the built-in icons. It is not necessary to provide all of them. The built-in icon will be used if not found in your icons directory.

*Example:*

    C:/apps/DoffenSSHTunnel/themes/icons/My Icons Theme 1/
                                       app_010_connected.png
                                       app_020_disconnected.png
                                       ca_010_btn_add_action.png
                                       ...
    C:/apps/DoffenSSHTunnel/themes/icons/My Icons Theme 2/
                                       app_010_connected.png
                                       app_020_disconnected.png
                                       ca_010_btn_add_action.png
                                       ...

*A new menu appears*

    Menu -> Settings -> Icons
                       Reload 
                       ------------------
                       Default (built-in)
                       My Icons Theme 1
                       My Icons Theme 2 

*You can also drop the icons directly in the 'icons' directory*

    C:/apps/DoffenSSHTunnel/themes/icons/
                                       app_010_connected.png
                                       app_020_disconnected.png
                                       ca_010_btn_add_action.png
                                       ...


*Then the menu will look like*

    Menu -> Settings -> Icons
                       Reload 
                       ------------------
                       Default (built-in)
                       Custom             <--- themes/icons (selected by default)
