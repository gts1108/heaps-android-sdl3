package org.haxe;

import org.libsdl.app.SDLActivity;


public class HashLinkActivity extends SDLActivity
{
    @Override
    protected String[] getLibraries() {
        return new String[] {
            "SDL3",
            "main"
        };
    }
}