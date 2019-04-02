Installing Under Windows:
    1. Required Video Codecs:
        Install K-Lite_Codec_Pack_1488_Basic.exe:
            The default options work fine, but you can check other codecs to enable support for more types of videos.

    2. Neuroscope:
        Remove any old versions of neuroscope prior to running the installer.
        Run "neuroscope-2.1.1-win64.exe" and install to the default directory.



Using the Video Features:
    0. Ensure that you have opened a .dat data file as usual.
    1. Click the "Video" Menu at the top of the screen and select "Link Video"
        - There is also a shortcut in the toolbar on the left side of the screen.
    2. Select the video that corresponds to the currently opened data file.
        - The data and video files must have been created on the same day.
    3. The video player window opens and the video is displayed.

Features:
    The video window's playback position is automatically synchronized to the position in the main data window.
        - Dragging the scroll bar at the bottom of the data window moves the video's playback position to the corresponding position.
        - Manually entering a "Start time" in the data window moves the video's playback position to the corresponding position.
        - Manually adjusting the video's playback position updates the data window's "Start time" as well (the sync is bidirectional).

    The left and right arrows near the bottom of the video window allow you to jump backward and forward in time along the duration of the video.
    The text field at the bottom of the video player allows you to enter a custom jump duration (in seconds). Fractional seconds (e.g. 2.5) are allowed.

    The play button allows you to playback the video at the recorded speed. This playback does NOT update the data window's "Start time" (the synchronization is temporarily disabled).


Notes:
    The temporal offset between the video and data windows is determined by the file/directory names of the opened files. The following conventions are expected:
        Data Folder Naming: Jasper_190319_125141
            {NAME}_{YYMMDD}_{HHmmss}

        Video Naming: Jasper_20190319_125207822.avi
            {NAME}_{YYYYMMDD}_{HHmmssSSS}.avi
