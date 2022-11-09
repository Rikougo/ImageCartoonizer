//
// Created by sakeiru on 11/7/2022.
//

#ifndef IMAGECARTOONIZER_KMEAN_HPP
#define IMAGECARTOONIZER_KMEAN_HPP

#include <random>

#include <Image.hpp>

namespace ImgCartoonizer {
    struct Point2D {int x, y; };
    typedef std::vector<unsigned char> Pixel;

    float SquareDist(const float* p_left, const float* p_right, int p_channels) {
        float l_result = 0;
        for (int i = 0; i < p_channels; i++) {
            l_result += (p_right[i] - p_left[i]) * (p_right[i] - p_left[i]);
        }
        return l_result;

    }

    Image Kmean(Image const &p_source, int p_k = 5, int p_iter = 10) {
        Image l_result{p_source};
        std::vector<std::vector<float>> l_centroids(p_k, std::vector<float>(p_source.channels));

        std::random_device l_device{};
        std::uniform_int_distribution l_widthDistrib{0, p_source.width};
        std::uniform_int_distribution l_heightDistrib(0, p_source.height);

        for(int i = 0; i < p_k; i++) {
            std::vector<float> l_value;
            l_value.reserve(p_source.channels);

            auto l_pixel = p_source.PixelAt(l_widthDistrib(l_device), l_heightDistrib(l_device));
            for(int j = 0; j < p_source.channels; j++) {
                l_value.push_back(l_pixel[i]);
            }

            l_centroids[i] = l_value;
        }

        std::vector<std::vector<Point2D>> l_clusters(p_k);
        for(int iter = 0; iter < p_iter; iter++) {
            for(auto &l_cluster : l_clusters) l_cluster.clear();
            for(int xPos = 0; xPos < p_source.width; xPos++) {
                for(int yPos = 0; yPos < p_source.height; yPos++) {
                    float l_minValue = std::numeric_limits<float>::max();
                    size_t l_minCluster = 0;

                    for(int c = 0; c < p_k; c++) {
                        float l_squareDist = SquareDist(
                                p_source.PixelAt(xPos, yPos),
                                l_centroids[c].data(),
                                p_source.channels);

                        if (l_squareDist < l_minValue) {
                            l_minValue = l_squareDist;
                            l_minCluster = c;
                        }
                    }

                    l_clusters[l_minCluster].push_back(Point2D{xPos, yPos});
                }
            }

            // update centroids
            for(int c = 0; c < p_k; c++) {
                std::vector<float> l_newCentroid(p_source.channels, 0);
                for(auto &l_position : l_clusters[c]) {
                    const float* l_pixel = p_source.PixelAt(l_position.x, l_position.y);
                    for(int i = 0; i < p_source.channels; i++) {
                        l_newCentroid[i] += l_pixel[i];
                    }
                }

                for(int i = 0; i < p_source.channels; i++) {
                    if (!l_clusters[c].empty())
                        l_centroids[c][i] = (l_newCentroid[i] / (float)l_clusters[c].size());
                }
            }
        }

        for(int c = 0; c < p_k; c++) {
            for(auto const &l_position : l_clusters[c]) {
                auto l_pixel = l_result.PixelAt(l_position.x, l_position.y);

                for(int i = 0; i < p_source.channels; i++) {
                    l_pixel[i] = l_centroids[c][i];
                }
            }
        }

        return l_result;
    };
}

#endif //IMAGECARTOONIZER_KMEAN_HPP
