#include <SDL2/SDL.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define SDL_AUDIO_BUFFER_SIZE 1024
#define MAX_AUDIO_FRAME_SIZE 192000

typedef struct
{
    AVFormatContext *pFormatCtx; // AVFormatContext structure, used to store audio and video format related information
    AVCodecContext *pCodecCtx;   // AVCodecContext structure, used to store codec related information
    AVCodec *pCodec;             // AVCodec structure, used to store codec information
    AVFrame *pFrame;             // AVFrame structure, used to store decoded video frames
    AVPacket packet;             // AVPacket structure, used to store audio and video data packets
    int videoStream;             // Index of the video stream
    int audioStream;             // Index of the audio stream
    int quit;                    // Quit flag
} VideoState;

// Global VideoState structure pointer, used to access the VideoState structure in the
// audio callback function
VideoState *global_video_state;

/**
 * Audio callback function, used to provide audio data to the SDL audio device
 *
 * @param userdata User data, i.e. VideoState structure pointer
 * @param stream Audio data buffer
 * @param len Length of the audio data buffer
 *
 */
void audio_callback(void *userdata, Uint8 *stream, int len)
{
    AVCodecContext *aCodecCtx =
        global_video_state->pFormatCtx->streams[global_video_state->audioStream]->codec; // Get the audio codec context
    int len1, audio_size;
    static uint8_t audio_buf[(MAX_AUDIO_FRAME_SIZE * 3) / 2]; // Static audio data buffer
    static unsigned int audio_buf_size = 0;                   // Size of the static audio data buffer
    static unsigned int audio_buf_index = 0;                  // Index of the static audio data buffer

    while (len > 0)
    {
        if (audio_buf_index >= audio_buf_size)
        { // If the audio data buffer has been played completely
            audio_size =
                audio_decode_frame(aCodecCtx, audio_buf, sizeof(audio_buf)); // Decode the next frame of audio data
            if (audio_size < 0)
            {                                         // If decoding fails
                audio_buf_size = 1024;                // Reset the size of the audio data buffer
                memset(audio_buf, 0, audio_buf_size); // Clear the audio data buffer
            }
            else
            {
                audio_buf_size = audio_size; // Update the size of the audio data buffer
            }
            audio_buf_index = 0; // Reset the index of the audio data buffer
        }
        len1 = audio_buf_size - audio_buf_index; // Calculate the length of audio data to be played this time
        if (len1 > len)
        {
            len1 = len;
        }
        memcpy(stream, (uint8_t *)audio_buf + audio_buf_index, len1); // Copy the audio data to the buffer
        len -= len1;
        stream += len1;
        audio_buf_index += len1;
    }
}

/**
 * Decode an audio frame
 *
 * @param aCodecCtx Audio codec context
 * @param audio_buf Audio data buffer
 * @param buf_size Size of the audio data buffer
 *
 * @return Size of the decoded audio data
 *
 */
int audio_decode_frame(AVCodecContext *aCodecCtx, uint8_t *audio_buf, int buf_size)
{
    static AVPacket pkt;                   // Static audio data packet
    static uint8_t *audio_pkt_data = NULL; // Static audio data packet data pointer
    static int audio_pkt_size = 0;         // Static audio data packet size
    static AVFrame frame;                  // Static audio frame
    int len1, data_size = 0;

    for (;;)
    {
        while (audio_pkt_size > 0)
        { // If there is still audio data packet that has not been decoded
            int got_frame = 0;
            len1 = avcodec_decode_audio4(aCodecCtx, &frame, &got_frame, &pkt); // Decode the audio data packet
            if (len1 < 0)
            {                       // If decoding fails
                audio_pkt_size = 0; // Reset the size of the audio data packet
                break;
            }
            audio_pkt_data += len1; // Update the data pointer of the audio data packet
            audio_pkt_size -= len1; // Update the size of the audio data packet
            if (got_frame)
            { // If decoding is successful
                data_size =
                    av_samples_get_buffer_size(NULL, aCodecCtx->channels, frame.nb_samples, aCodecCtx->sample_fmt,
                                               1);           // Calculate the size of the audio data
                memcpy(audio_buf, frame.data[0], data_size); // Copy the audio data to the buffer
            }
            if (data_size <= 0)
            { // If the size of the audio data is 0
                continue;
            }
            return data_size; // Return the size of the audio data
        }
        if (pkt.data)
        {                         // If there is still audio data packet that has not been decoded
            av_free_packet(&pkt); // Free the audio data packet
        }
        if (global_video_state->quit)
        { // If the quit flag is true
            return -1;
        }
        if (av_read_frame(global_video_state->pFormatCtx, &pkt) < 0)
        { // Read the next audio data packet
            return -1;
        }
        if (pkt.stream_index == global_video_state->audioStream)
        { // If it is an audio stream data packet
            audio_pkt_data = pkt.data;
            audio_pkt_size = pkt.size;
        }
    }
}
/**
 * Decode a video frame
 *
 * @param is VideoState structure pointer, containing audio and video related information
 * @param frame Decoded video frame
 *
 * @return Size of the decoded video data
 *
 */
int video_decode_frame(VideoState *is, AVFrame *frame)
{
    int len, got_picture;
    static struct SwsContext *img_convert_ctx;
    AVCodecContext *pCodecCtx = is->pCodecCtx;
    AVPacket *packet = &is->packet;

    for (;;)
    {
        while (packet->size > 0)
        {
            got_picture = 0;
            len = avcodec_decode_video2(pCodecCtx, frame, &got_picture, packet); // Decode the video data packet
            if (len < 0)
            {
                packet->size = 0;
                break;
            }
            packet->data += len;
            packet->size -= len;
            if (got_picture)
            {
                if (!img_convert_ctx)
                {
                    img_convert_ctx =
                        sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt, pCodecCtx->width,
                                       pCodecCtx->height, AV_PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL,
                                       NULL); // Initialize the SwsContext structure for pixel format conversion
                }
                sws_scale(img_convert_ctx, (const uint8_t *const *)frame->data, frame->linesize, 0, pCodecCtx->height,
                          frame->data, frame->linesize); // Pixel format conversion
                return 1;                                // Return 1 to indicate decoding success
            }
        }
        if (packet->data)
        {
            av_free_packet(packet); // Free the video data packet
        }
        if (global_video_state->quit)
        {
            return -1; // Return -1 to indicate quitting
        }
        if (av_read_frame(is->pFormatCtx, packet) < 0) // Read the next video data packet
        {
            return -1; // Return -1 to indicate reading failure
        }
        if (packet->stream_index == is->videoStream) // If it is a video stream data packet
        {
            break;
        }
    }
    return 0; // Return 0 to indicate more data is needed for decoding success
}

/**
 * Display a video frame
 *
 * @param is VideoState structure pointer, containing audio and video related information
 *
 */
void video_display(VideoState *is)
{
    SDL_Rect rect;
    AVFrame *pFrame = is->pFrame; // Video frame
    SDL_Texture *texture;         // SDL texture
    SDL_Renderer *renderer;       // SDL renderer
    SDL_Rect sdlRect;

    renderer = SDL_CreateRenderer(global_video_state->window, -1, 0); // Create SDL renderer
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_YV12, SDL_TEXTUREACCESS_STREAMING, pFrame->width,
                                pFrame->height); // Create SDL texture
    sdlRect.x = 0;
    sdlRect.y = 0;
    sdlRect.w = pFrame->width;
    sdlRect.h = pFrame->height;

    SDL_UpdateYUVTexture(texture, &sdlRect, pFrame->data[0], pFrame->linesize[0], pFrame->data[1], pFrame->linesize[1],
                         pFrame->data[2], pFrame->linesize[2]); // Update SDL texture
    SDL_RenderClear(renderer);                                  // Clear renderer
    SDL_RenderCopy(renderer, texture, NULL, NULL);              // Copy texture to renderer
    SDL_RenderPresent(renderer);                                // Show renderer content
    SDL_DestroyTexture(texture);                                // Destroy texture
    SDL_DestroyRenderer(renderer);                              // Destroy renderer
}

/**
 * Video thread function
 *
 * @param arg VideoState structure pointer, containing audio and video related information
 *
 * @return None
 *
 */
void *video_thread(void *arg)
{
    VideoState *is = (VideoState *)arg;                     // VideoState structure pointer
    AVFrame *pFrame = av_frame_alloc();                     // Video frame
    int frameFinished;                                      // Flag indicating whether the video frame is decoded
    while (av_read_frame(is->pFormatCtx, &is->packet) >= 0) // Read audio and video data packets
    {
        if (is->packet.stream_index == is->videoStream) // If it is a video stream data packet
        {
            if (video_decode_frame(is, pFrame) > 0) // Decode the video frame
            {
                video_display(is); // Display the video frame
            }
        }
        av_free_packet(&is->packet); // Free the audio and video data packets
    }
    av_free(pFrame); // Free the video frame
    return 0;
}

/**
 * Audio thread function
 *
 * @param arg VideoState structure pointer, containing audio and video related information
 *
 * @return None
 *
 */
void *audio_thread(void *arg)
{
    VideoState *is = (VideoState *)arg;                                          // VideoState structure pointer
    AVCodecContext *aCodecCtx = is->pFormatCtx->streams[is->audioStream]->codec; // Audio codec context
    SDL_AudioSpec wanted_spec, spec;                                             // SDL audio specifications
    int len;

    wanted_spec.freq = aCodecCtx->sample_rate;  // Sampling rate
    wanted_spec.format = AUDIO_S16SYS;          // Audio format
    wanted_spec.channels = aCodecCtx->channels; // Number of channels
    wanted_spec.silence = 0;
    wanted_spec.samples = SDL_AUDIO_BUFFER_SIZE; // Audio buffer size
    wanted_spec.callback = audio_callback;       // Audio callback function
    wanted_spec.userdata = is;

    if (SDL_OpenAudio(&wanted_spec, &spec) < 0) // Open SDL audio device
    {
        fprintf(stderr, "SDL_OpenAudio: %s\n", SDL_GetError());
        return NULL;
    }

    global_video_state = is; // Global VideoState structure pointer

    SDL_PauseAudio(0); // Resume audio playback

    while (!is->quit) // If the quit flag is false
    {
        SDL_Delay(100); // Delay for 100 milliseconds
    }

    SDL_CloseAudio(); // Close SDL audio device
    return 0;
}

int main(int argc, char *argv[])
{
    VideoState *is;                 // VideoState structure pointer
    pthread_t video_tid, audio_tid; // Video thread and audio thread ID

    is = av_mallocz(sizeof(VideoState)); // Allocate memory for VideoState structure

    av_register_all(); // Register all FFmpeg codecs and formats

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER)) // Initialize SDL
    {
        fprintf(stderr, "Could not initialize SDL - %s\n", SDL_GetError());
        exit(1);
    }

    is->pFormatCtx = avformat_alloc_context(); // Allocate memory for AVFormatContext structure

    if (avformat_open_input(&is->pFormatCtx, argv[1], NULL, NULL) != 0) // Open video file
    {
        fprintf(stderr, "Could not open video file\n");
        exit(1);
    }

    if (avformat_find_stream_info(is->pFormatCtx, NULL) < 0) // Find audio and video stream information
    {
        fprintf(stderr, "Could not find stream information\n");
        exit(1);
    }

    av_dump_format(is->pFormatCtx, 0, argv[1], 0); // Print audio and video format information

    is->videoStream = -1;
    is->audioStream = -1;

    for (int i = 0; i < is->pFormatCtx->nb_streams; i++) // Traverse audio and video streams
    {
        if (is->pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO &&
            is->videoStream < 0) // If it's a video stream
        {
            is->videoStream = i; // Record the index of the video stream
        }
        if (is->pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO &&
            is->audioStream < 0) // If it's an audio stream
        {
            is->audioStream = i; // Record the index of the audio stream
        }
    }

    if (is->videoStream == -1) // If no video stream is found
    {
        fprintf(stderr, "Could not find video stream\n");
        exit(1);
    }

    if (is->audioStream == -1) // If no audio stream is found
    {
        fprintf(stderr, "Could not find audio stream\n");
        exit(1);
    }

    is->pCodecCtx = is->pFormatCtx->streams[is->videoStream]->codec; // Video codec context
    is->pCodec = avcodec_find_decoder(is->pCodecCtx->codec_id);      // Find video decoder

    if (avcodec_open2(is->pCodecCtx, is->pCodec, NULL) < 0) // Open video decoder
    {
        fprintf(stderr, "Could not open codec\n");
        exit(1);
    }

    SDL_CreateWindowAndRenderer(is->pCodecCtx->width, is->pCodecCtx->height, 0, &global_video_state->window,
                                &global_video_state->renderer); // Create SDL window and renderer

    av_init_packet(&is->packet); // Initialize audio and video packet

    pthread_create(&video_tid, NULL, video_thread, (void *)is); // Create video thread
    pthread_create(&audio_tid, NULL, audio_thread, (void *)is); // Create audio thread

    pthread_join(video_tid, NULL); // Wait for video thread to finish
    pthread_join(audio_tid, NULL); // Wait for audio thread to finish

    SDL_DestroyWindow(global_video_state->window); // Destroy SDL window
    avcodec_close(is->pCodecCtx);                  // Close video decoder
    avformat_close_input(&is->pFormatCtx);         // Close audio and video file
    av_free(is);                                   // Free VideoState structure memory

    return 0;
}