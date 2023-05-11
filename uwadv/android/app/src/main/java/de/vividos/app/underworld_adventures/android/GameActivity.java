package de.vividos.app.underworld_adventures.android;

import android.os.Bundle;
import org.libsdl.app.SDLActivity;

public class GameActivity extends SDLActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
    }

    @Override
    protected String[] getLibraries() {
        return new String[] {
            "SDL2",
            "SDL2_mixer",
            "uwadv_android"
        };
    }
}
