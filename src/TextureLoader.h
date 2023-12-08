#ifndef TEXTURELOADER_H
#define TEXTURELOADER_H

#include "FuryLogger.h"

#define GLEW_STATIC
#include <GL/glew.h>
#include <glm/glm.hpp>

#include <SOIL.h>

#include <vector>
#include <string>

#include <QDebug>
#include <QMutex>
#include <QMutexLocker>


class TextureLoader {
public:
    static unsigned char* LoadDataFromFile(const QString& filename, int& width, int& height) {
        static QMutex mutexLoadImage;
        QMutexLocker locker(&mutexLoadImage);
        auto result = SOIL_load_image(qUtf8Printable(filename), &width, &height, 0, SOIL_LOAD_RGBA);
        return result;
    };

    static GLuint BindDataToCubemap(QVector<unsigned char*> data_array, QVector<int>& width_array, QVector<int>& height_array) {
        unsigned int textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

        for (unsigned int i = 0; i < data_array.size(); i++)
        {
            if (data_array[i])
            {
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, width_array[i], height_array[i], 0, GL_RGBA, GL_UNSIGNED_BYTE, data_array[i]);
                SOIL_free_image_data(data_array[i]);
            }
            else
            {
                Debug("Cubemap texture failed to load at path: ") << data_array[i];
                SOIL_free_image_data(data_array[i]);
            }
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

        return textureID;
    }

    static GLuint BindDataToTexture(unsigned char* data, int width, int height) {
        unsigned int textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);

        if (data)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
            SOIL_free_image_data(data);
        }
        else
        {
            qDebug() << "Texture failed to load: " << width << " " << height;
            SOIL_free_image_data(data);
        }

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);

        return textureID;
    }
};

#endif // TEXTURELOADER_H
