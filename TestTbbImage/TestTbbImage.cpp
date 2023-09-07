#include <iostream>
#include <tbb/parallel_pipeline.h>
#include <opencv2/opencv.hpp>


bool image_read = false;

class InputImage {
public:

    InputImage(std::string path_to_source_image) : path_to_source_image_(path_to_source_image) {}

    cv::Mat operator()(tbb::flow_control& fc) const{

        cv::Mat image = cv::imread(path_to_source_image_, cv::IMREAD_COLOR);

        // Verifica se l'immagine è stata letta correttamente
        if (image.empty() ) {
            std::cout << "Unable to read input image" << std::endl;
            system("pause");
            //fc.stop();
        }
        else if (image_read) {
            //fc.stop();
        }
        else {
            image_read = true;
            cv::imshow("Immagine letta", image);
        }

        return image;
    }

    std::string path_to_source_image_;
};


class ResizeImage {

public:

    cv::Mat operator()(cv::Mat image) const {

        cv::waitKey(0);

        cv::Mat resizedImage;
        cv::resize(image, resizedImage, cv::Size(300, 200));

        return resizedImage;
    }
};

class OutputImage {

public:

    OutputImage(std::string path_to_target_image) : path_to_target_image_(path_to_target_image) {}

    void operator()(cv::Mat image) const {

        cv::imshow("Immagine modificata", image);
        cv::imwrite(path_to_target_image_, image);
    }

    std::string path_to_target_image_;
};

void RunPipeline(int ntoken, std::string path_to_source_image, std::string path_to_targer_image) {

    tbb::filter<void, cv::Mat> input_filter(tbb::filter_mode::serial_in_order,
        InputImage(path_to_source_image));

    tbb::filter<cv::Mat, cv::Mat> resize_image_filter(tbb::filter_mode::serial_in_order,
        ResizeImage());

    tbb::filter<cv::Mat, void> output_filter(tbb::filter_mode::serial_in_order,
        OutputImage(path_to_targer_image));

    tbb::filter<void, void> f = input_filter & resize_image_filter & output_filter;
    tbb::parallel_pipeline(ntoken, f);
}

int main()
{
    RunPipeline(1, "../SourceImage/Foto.jpeg", "../TargetImage/Foto_ridimensionata.jpeg");
    cv::waitKey(0);
}
