#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>
using namespace std;

struct Pixels {
    unsigned char blue;
    unsigned char green;
    unsigned char red;
};
struct Header {
    char idLength;
    char colorMapType;
    char dataTypeCode;
    short colorMapOrigin;
    short colorMapLength;
    char colorMapDepth;
    short xOrigin;
    short yOrigin;
    short width;
    short height;
    char bitsPerPixel;
    char imageDescriptor;
    vector<Pixels> pixels;
};

void Read(string filename, Header &header) {
    ifstream openFile(filename, ios_base::binary);
    openFile.read(reinterpret_cast<char*>(&header.idLength), sizeof(header.idLength));
    openFile.read(reinterpret_cast<char*>(&header.colorMapType), sizeof(header.colorMapType));
    openFile.read(reinterpret_cast<char*>(&header.dataTypeCode), sizeof(header.dataTypeCode));
    openFile.read(reinterpret_cast<char*>(&header.colorMapOrigin), sizeof(header.colorMapOrigin));
    openFile.read(reinterpret_cast<char*>(&header.colorMapLength), sizeof(header.colorMapLength));
    openFile.read(reinterpret_cast<char*>(&header.colorMapDepth), sizeof(header.colorMapDepth));
    openFile.read(reinterpret_cast<char*>(&header.xOrigin), sizeof(header.xOrigin));
    openFile.read(reinterpret_cast<char*>(&header.yOrigin), sizeof(header.yOrigin));
    openFile.read(reinterpret_cast<char*>(&header.width), sizeof(header.width));
    openFile.read(reinterpret_cast<char*>(&header.height), sizeof(header.height));
    openFile.read(reinterpret_cast<char*>(&header.bitsPerPixel), sizeof(header.bitsPerPixel));
    openFile.read(reinterpret_cast<char*>(&header.imageDescriptor), sizeof(header.imageDescriptor));
    for (int i = 0; i < header.width * header.height; i++) {
        Pixels pixel;
        openFile.read(reinterpret_cast<char*>(&pixel.blue), sizeof(pixel.blue));
        openFile.read(reinterpret_cast<char*>(&pixel.green), sizeof(pixel.green));
        openFile.read(reinterpret_cast<char*>(&pixel.red), sizeof(pixel.red));
        header.pixels.push_back(pixel);
    }
}
    void Write(string filename, Header &header){
        ofstream output(filename, ios_base::binary);
        output.write(reinterpret_cast<char*>(&header.idLength), sizeof(header.idLength));
        output.write(reinterpret_cast<char*>(&header.colorMapType), sizeof(header.colorMapType));
        output.write(reinterpret_cast<char*>(&header.dataTypeCode), sizeof(header.dataTypeCode));
        output.write(reinterpret_cast<char*>(&header.colorMapOrigin), sizeof(header.colorMapOrigin));
        output.write(reinterpret_cast<char*>(&header.colorMapLength), sizeof(header.colorMapLength));
        output.write(reinterpret_cast<char*>(&header.colorMapDepth), sizeof(header.colorMapDepth));
        output.write(reinterpret_cast<char*>(&header.xOrigin), sizeof(header.xOrigin));
        output.write(reinterpret_cast<char*>(&header.yOrigin), sizeof(header.yOrigin));
        output.write(reinterpret_cast<char*>(&header.width), sizeof(header.width));
        output.write(reinterpret_cast<char*>(&header.height), sizeof(header.height));
        output.write(reinterpret_cast<char*>(&header.bitsPerPixel), sizeof(header.bitsPerPixel));
        output.write(reinterpret_cast<char*>(&header.imageDescriptor), sizeof(header.imageDescriptor));
        for (int i = 0; i < header.pixels.size(); i++) {
            output.write(reinterpret_cast<char*>(&header.pixels[i].blue), sizeof(header.pixels[i].blue));
            output.write(reinterpret_cast<char*>(&header.pixels[i].green), sizeof(header.pixels[i].green));
            output.write(reinterpret_cast<char*>(&header.pixels[i].red), sizeof(header.pixels[i].red));
        }
    }
Header Multiply(Header &layer1, Header &layer2) {
    Header result;
    result = layer1;
    result.pixels.resize(layer1.width * layer1.height);
    for (int i = 0; i < layer1.width * layer1.height; i++) {
        result.pixels[i].red = ((layer1.pixels[i].red/255.0f) * (layer2.pixels[i].red/255.0f))*255+0.5f;
        result.pixels[i].green = ((layer1.pixels[i].green/255.0f) * (layer2.pixels[i].green/255.0f))*255+0.5f;
        result.pixels[i].blue = ((layer1.pixels[i].blue/255.0f) * (layer2.pixels[i].blue/255.0f))*255+0.5f;
    }
    return result;

}
Header Screen(Header &layer1, Header &layer2) {
    Header result;
    result = layer1;
    result.pixels.resize(layer1.width * layer1.height);
    for (int i = 0; i < layer1.width * layer1.height; i++) {
        result.pixels[i].red = (255.0f-((1.0f-layer2.pixels[i].red/255.0f) * (1.0f-layer1.pixels[i].red/255.0f))*255+0.5f);
        result.pixels[i].green = (255.0f-((1.0f-layer2.pixels[i].green/255.0f) * (1.0f-layer1.pixels[i].green/255.0f))*255+0.5f);
        result.pixels[i].blue = (255.0f-((1.0f-layer2.pixels[i].blue/255.0f) * (1.0f-layer1.pixels[i].blue/255.0f))*255+0.5f);
    }
    return result;
}
Header Subtract(Header &layer1, Header &layer2) {
    Header result;
    result = layer1;
    result.pixels.resize(layer1.width * layer1.height);
    for (int i = 0; i < layer1.width * layer1.height; i++) {
        result.pixels[i].red = max(0, layer1.pixels[i].red - layer2.pixels[i].red);
        result.pixels[i].green = max(0, layer1.pixels[i].green - layer2.pixels[i].green);
        result.pixels[i].blue = max(0, layer1.pixels[i].blue - layer2.pixels[i].blue);
    }
    return result;
}
Header Overlay(Header &layer1, Header &layer2) {
    Header result;
    result = layer1;
    result.pixels.resize(layer1.width * layer1.height);
    for (int i = 0; i < layer1.width * layer1.height; i++) {
        if (layer2.pixels[i].red/255.0 <= 0.5) {
            result.pixels[i].red = (2*(layer1.pixels[i].red/255.0f) * (layer2.pixels[i].red/255.0f)*255)+0.5f;
        } else if (layer2.pixels[i].red/255.0 > 0.5) {
            result.pixels[i].red = 255.0f-(2*((1.0f-layer2.pixels[i].red/255.0f) * (1.0f-layer1.pixels[i].red/255.0f)*255))+0.5f;
        }
        if (layer2.pixels[i].green/255.0 <= 0.5) {
            result.pixels[i].green = (2*(layer1.pixels[i].green/255.0f) * (layer2.pixels[i].green/255.0f)*255)+0.5f;
        } else if (layer2.pixels[i].green/255.0 > 0.5) {
            result.pixels[i].green = 255.0f-(2*((1.0f-layer2.pixels[i].green/255.0f) * (1.0f-layer1.pixels[i].green/255.0f)*255))+0.5f;
        }
        if (layer2.pixels[i].blue/255.0 <= 0.5) {
            result.pixels[i].blue = (2*(layer1.pixels[i].blue/255.0f) * (layer2.pixels[i].blue/255.0f)*255)+0.5f;
        } else if (layer2.pixels[i].blue/255.0 > 0.5) {
            result.pixels[i].blue = 255.0f-(2*((1.0f-layer2.pixels[i].blue/255.0f) * (1.0f-layer1.pixels[i].blue/255.0f)*255))+0.5f;
        }
    }
    return result;

}
Header Rotate(Header &layer1) {
    Header result;
    result = layer1;
    result.pixels.resize(layer1.width * layer1.height);
    reverse(result.pixels.begin(), result.pixels.end());
    // for (int i = layer1.width * layer1.height - 1; i >= 0; i--) {
    //     result.pixels[(layer1.width * layer1.height) - 1 - i].red = layer1.pixels[i].red;
    //     result.pixels[(layer1.width * layer1.height) - 1 - i].green = layer1.pixels[i].green;
    //     result.pixels[(layer1.width * layer1.height) - 1 - i].blue = layer1.pixels[i].blue;
    // }
    // for (int i = 0; i < layer1.width * layer1.height; i++) {
    //     result.pixels[i].red = layer1.pixels[layer1.width * layer1.height - i - 1].red;
    //     result.pixels[i].green = layer1.pixels[layer1.width * layer1.height - i - 1].green;
    //     result.pixels[i].blue = layer1.pixels[layer1.width * layer1.height - i - 1].blue;
    // }
    return result;
}
Header ScaleGreen(Header &layer1, int scale) {
    Header result;
    result = layer1;
    result.pixels.resize(layer1.width * layer1.height);
    for (int i = 0; i< result.pixels.size(); i++) {
        result.pixels[i].green = min(255, scale * layer1.pixels[i].green);
    }
    return result;
}
Header ScaleRed(Header &layer1, int scale) {
    Header result;
    result = layer1;
    result.pixels.resize(layer1.width * layer1.height);
    for (int i = 0; i< result.pixels.size(); i++) {
        result.pixels[i].red = min(255, scale * result.pixels[i].red);
    }
    return result;
}
Header ScaleBlue(Header &layer1, int scale) {
    Header result;
    result = layer1;
    result.pixels.resize(layer1.width * layer1.height);
    for (int i = 0; i< result.pixels.size(); i++) {
        result.pixels[i].blue = min(255, scale * result.pixels[i].blue);
    }
    return result;
}
Header AddRed(Header &layer, int amount) {
    Header result;
    result = layer;
    result.pixels.resize(layer.width * layer.height);
    for (int i = 0; i< result.pixels.size(); i++) {
        if (amount > 0) {
            result.pixels[i].red = min(255, result.pixels[i].red + amount);
        } else if (amount < 0) {
            result.pixels[i].red = max(0, result.pixels[i].red + amount);
        }
    }
    return result;
}
Header AddGreen(Header &layer, int amount) {
    Header result;
    result = layer;
    result.pixels.resize(layer.width * layer.height);
    for (int i = 0; i< result.pixels.size(); i++) {
        if (amount > 0) {
            result.pixels[i].green = min(255, result.pixels[i].green + amount);
        } else if (amount < 0) {
            result.pixels[i].green = max(0, result.pixels[i].green + amount);
        }
    }
    return result;
}
Header AddBlue(Header &layer, int amount) {
    Header result;
    result = layer;
    result.pixels.resize(layer.width * layer.height);
    for (int i = 0; i< result.pixels.size(); i++) {
        if (amount > 0) {
            result.pixels[i].blue = min(255, result.pixels[i].blue + amount);
        } else if (amount < 0) {
            result.pixels[i].blue = max(0, result.pixels[i].blue + amount);
        }
    }
    return result;
}
Header OnlyRed(Header &layer) {
    Header result;
    result = layer;
    result.pixels.resize(layer.width * layer.height);
    for (int i = 0; i< result.pixels.size(); i++) {
        result.pixels[i].blue = result.pixels[i].red;
        result.pixels[i].green = result.pixels[i].red;
    }
    return result;
}
Header OnlyGreen(Header &layer) {
    Header result;
    result = layer;
    result.pixels.resize(layer.width * layer.height);
    for (int i = 0; i< result.pixels.size(); i++) {
        result.pixels[i].blue = result.pixels[i].green;
        result.pixels[i].red = result.pixels[i].green;
    }
    return result;
}
Header OnlyBlue(Header &layer) {
    Header result;
    result = layer;
    result.pixels.resize(layer.width * layer.height);
    for (int i = 0; i< result.pixels.size(); i++) {
        result.pixels[i].green = result.pixels[i].blue;
        result.pixels[i].red = result.pixels[i].blue;
    }
    return result;
}
Header Combine(Header &red, Header &green, Header &blue) {
    Header result;
    result = red;
    result.pixels.resize(red.width * red.height);
    for (int i = 0; i< result.pixels.size(); i++) {
        result.pixels[i].green = green.pixels[i].green;
        result.pixels[i].blue = blue.pixels[i].blue;
    }
    return result;
}


int main(int argc, const char* argv[]) {
    Header trackingImage;
    vector<string> args;
    int errorcount = 0;
    for (int i = 1; i < argc; i++) {
        args.push_back(argv[i]);
    }
    if (args.empty() || (args.size() == 1 && args[0] == "--help")) {
        cout << "Project 2: Image Processing, Fall 2024\n\n";
        cout << "Usage:\n\t./project2.out [output] [firstImage] [method] [...]\n";
        return 0;
    }
    if (args[0].find("output/")) {
        cerr << "Invalid file name.\n";
        errorcount++;
        if (args.size() >= 2){
            for (int j = 1; j <= 1; j++) {
                if (args[j].find(".tga") == string::npos || args[j].find("input/") == string::npos) {
                    // cout << args[i].find("tga");
                    // cout << args[i].find("input/") << endl;
                    cerr << "Invalid file name.\n";
                    errorcount++;
                } else {
                    ifstream file(args[j]);
                    if (!file.is_open()) {
                        file.close();
                        cerr << "File does not exist.";
                        errorcount++;
                    }
                }
            }
        }
    }
    if (args.size() >= 3){
    if (args[2] != "scalered" && args[2] != "scaleblue" && args[2] != "scalegreen"
        && args[2] != "addred" && args[2] != "addblue" && args[2] != "addgreen"
        && args[2] != "multiply" && args[2] != "subtract" && args[2] != "screen" && args[2] != "overlay"
        && args[2] != "combine" && args[2] != "flip" && args[2] != "onlyred" &&
        args[2] != "onlyblue" && args[2] != "onlygreen")  {
        cerr << "Invalid method name.";
        errorcount++;
        }
}
    if (errorcount >0 && args.size() <= 2) {
        return 1;
    }

    int i = 2;
    while (i < args.size()) {
        if (args[i] == "multiply") {
            if (i+1 >= args.size()) {
                cerr << "Missing argument.\n";
                return 1;
            } else {
                if (i == 2) {
                    Header toplayer;
                    Header bottomlayer;
                    Read(args[1], toplayer);
                    Read(args[3], bottomlayer);
                    trackingImage = Multiply(toplayer, bottomlayer);
                    cout << "Multiplying " << args[1] << " and " << args[3] << " ...\n";
                } else {
                    Header bottomlayer;
                    Read(args[i+1], bottomlayer);
                    Header result = Multiply(trackingImage, bottomlayer);
                    trackingImage = result;
                    cout << "... and multiplying " << args[i+1] << " from previous step ...\n";
                }
            }
            i+=2;
        }
        else if (args[i] == "subtract") {
            if (i+1 >= args.size()) {
                cerr << "Missing argument.\n";
                return 1;
            } else {
                if (i == 2) {
                    Header toplayer;
                    Header bottomlayer;
                    Read(args[1], toplayer);
                    Read(args[3], bottomlayer);
                    trackingImage = Subtract(toplayer, bottomlayer);
                    cout << "Subtracting " << args[1] << " and " << args[3] << " ...\n";
                } else {
                    Header bottomlayer;
                    Read(args[i+1], bottomlayer);
                    Header result = Subtract(trackingImage, bottomlayer);
                    trackingImage = result;
                    cout << "... and subtracting " << args[i+1] << " from previous step ...\n";
                }
            }
            i+=2;
        }
        else if (args[i] == "overlay") {
            if (i+1 >= args.size()) {
                cerr << "Missing argument.\n";
                return 1;
            } else {
                if (i == 2) {
                    Header toplayer;
                    Header bottomlayer;
                    Read(args[1], toplayer);
                    Read(args[3], bottomlayer);
                    trackingImage = Overlay(toplayer, bottomlayer);
                    cout << "Overlaying " << args[1] << " and " << args[3] << " ...\n";
                } else {
                    Header bottomlayer;
                    Read(args[i+1], bottomlayer);
                    Header result = Overlay(trackingImage, bottomlayer);
                    trackingImage = result;
                    cout << "... and overlaying " << args[i+1] << " from previous step ...\n";
                }
            }
            i+=2;
        }
        else if (args[i] == "screen") {
            if (i+1 >= args.size()) {
                cerr << "Missing argument.\n";
                return 1;
            } else {
                if (i == 2) {
                    Header toplayer;
                    Header bottomlayer;
                    Read(args[1], toplayer);
                    Read(args[3], bottomlayer);
                    trackingImage = Screen(toplayer, bottomlayer);
                    cout << "Screening " << args[1] << " and " << args[3] << " ...\n";
                } else {
                    Header toplayer;
                    Read(args[i+1], toplayer);
                    Header result = Screen(toplayer, trackingImage);
                    trackingImage = result;
                    cout << "... and screening " << args[i+1] << " from previous step ...\n";
                }
            }
            i+=2;
        }
        else if (args[i] == "combine") {
            if (i+2 >= args.size()) {
                cerr << "Missing argument.\n";
                return 1;
            } else {
                if (i==2) {
                    Header toplayer;
                    Header middlelayer;
                    Header bottomlayer;
                    Read(args[1], toplayer);
                    Read(args[3], middlelayer);
                    Read(args[4], bottomlayer);
                    trackingImage = Combine(toplayer, middlelayer, bottomlayer);
                    cout << "Combining channels of " << args[1] << ", " << args[3] << ", and " << args[4] << " ...\n";
                } else {
                    Header middlelayer;
                    Header bottomlayer;
                    Read(args[i+1], middlelayer);
                    Read(args[i+2], bottomlayer);
                    Header result = Combine(trackingImage, middlelayer, bottomlayer);
                    trackingImage = result;
                    cout << "... and combining " << args[i+1] << ", " << args[i+2] << ", and previous step ...\n";
                }
            }
            i+=3;
        }
        else if (args[i] == "flip") {
            if (i == 2) {
                Header layer;
                Read(args[1], layer);
                trackingImage = Rotate(layer);
                cout << "Flipping " << args[1] << " ...\n";
            } else {
                Header result = Rotate(trackingImage);
                trackingImage = result;
                cout << "... and flipping output of previous step ...\n";
            }
            i++;
        }
        else if (args[i] == "onlyred") {
            if (i == 2) {
                Header layer;
                Read(args[1], layer);
                trackingImage = OnlyRed(layer);
                cout << "Retrieving red channel from " << args[1] << " ...\n";
            } else {
                Header result = OnlyRed(trackingImage);
                trackingImage = result;
                cout << "... and retrieving red channel from previous step ...\n";
            }
            i++;
        }
        else if (args[i] == "onlygreen") {
            if (i == 2) {
                Header layer;
                Read(args[1], layer);
                trackingImage = OnlyGreen(layer);
                cout << "Retrieving green channel from " << args[1] << " ...\n";
            } else {
                Header result = OnlyGreen(trackingImage);
                trackingImage = result;
                cout << "... and retrieving green channel from previous step ...\n";
            }
            i++;
        }
        else if (args[i] == "onlyblue") {
            if (i == 2) {
                Header layer;
                Read(args[1], layer);
                trackingImage = OnlyBlue(layer);
                cout << "Retrieving blue channel from " << args[1] << " ...\n";
            } else {
                Header result = OnlyBlue(trackingImage);
                trackingImage = result;
                cout << "... and retrieving blue channel from previous step ...\n";
            }
            i++;
        }
        else if (args[i] == "addred") {
            if (i+1 >= args.size()) {
                cerr << "Missing argument.\n";
                return 1;
            } else if (args[i+1][0] != '-' && !isdigit(args[i+1][0]) ||
                args[i+1][0] == '-' && !isdigit(args[i+1][1])) {
                cerr << "Invalid argument, expected number.";
                return 1;
                }
            else {
                int value = stoi(args[i+1]);
                if (i == 2) {
                    Header layer;
                    Read(args[1], layer);
                    trackingImage = AddRed(layer, value);
                    cout << "Adding +" << value << " to red channel of " << args[1] << " ...\n";
                } else {
                    Header result = AddRed(trackingImage, value);
                    trackingImage = result;
                    cout << "... and adding +" << value << " to red channel of previous step ...\n";
                }
            }
            i+=2;
        }
        else if (args[i] == "addgreen") {
            if (i+1 >= args.size()) {
                cerr << "Missing argument.\n";
                return 1;
            } else if (args[i+1][0] != '-' && !isdigit(args[i+1][0]) ||
                args[i+1][0] == '-' && !isdigit(args[i+1][1])) {
                cerr << "Invalid argument, expected number.";
                return 1;
                }
            else {
                int value = stoi(args[i+1]);
                if (i == 2) {
                    Header layer;
                    Read(args[1], layer);
                    trackingImage = AddGreen(layer, value);
                    cout << "Adding +" << value << " to green channel of " << args[1] << " ...\n";
                } else {
                    Header result = AddGreen(trackingImage, value);
                    trackingImage = result;
                    cout << "... and adding +" << value << " to green channel of previous step ...\n";
                }
            }
            i+=2;
        }
        else if (args[i] == "addblue") {
            if (i+1 >= args.size()) {
                cerr << "Missing argument.\n";
                return 1;
            } else if ((args[i+1][0] != '-' && !isdigit(args[i+1][0])) ||
                (args[i+1][0] == '-' && !isdigit(args[i+1][1]))) {
                cerr << "Invalid argument, expected number.";
                return 1;
                }
            else {
                int value = stoi(args[i+1]);
                if (i == 2) {
                    Header layer;
                    Read(args[1], layer);
                    trackingImage = AddBlue(layer, value);
                    cout << "Adding +" << value << " to blue channel of " << args[1] << " ...\n";
                } else {
                    Header result = AddBlue(trackingImage, value);
                    trackingImage = result;
                    cout << "... and adding +" << value << " to blue channel of previous step ...\n";
                }
            }
            i+=2;
        }
        else if (args[i] == "scalered") {
            if (i+1 >= args.size()) {
                cerr << "Missing argument.\n";
                return 1;
            } else if (!isdigit(args[i+1][0])) {
                cerr << "Invalid argument, expected number.";
                return 1;
            }
            else {
                int value = stoi(args[i+1]);
                if (i == 2) {
                    Header layer;
                    Read(args[1], layer);
                    trackingImage = ScaleRed(layer, value);
                    cout << "Scaling the red channel of " << args[i] << " by " << value << " ...\n";
                } else {
                    Header result = ScaleRed(trackingImage, value);
                    trackingImage = result;
                    cout << "... and scaling the red channel of previous step by " << value << " ...\n";
                }
            }
            i+=2;
        }
        else if (args[i] == "scaleblue") {
            if (i+1 >= args.size()) {
                cerr << "Missing argument.\n";
                return 1;
            } else if (!isdigit(args[i+1][0])) {
                cerr << "Invalid argument, expected number.";
                return 1;
            }
            else {
                int value = stoi(args[i+1]);
                if (i == 2) {
                    Header layer;
                    Read(args[1], layer);
                    trackingImage = ScaleBlue(layer, value);
                    cout << "Scaling the blue channel of " << args[i] << " by " << value << " ...\n";
                } else {
                    Header result = ScaleBlue(trackingImage, value);
                    trackingImage = result;
                    cout << "... and scaling the blue channel of previous step by " << value << " ...\n";
                }
            }
            i+=2;
        }
        else if (args[i] == "scalegreen") {
            if (i+1 >= args.size()) {
                cerr << "Missing argument.\n";
                return 1;
            } else if (!isdigit(args[i+1][0])) {
                cerr << "Invalid argument, expected number.";
                return 1;
            }
            else {
                int value = stoi(args[i+1]);
                if (i == 2) {
                    Header layer;
                    Read(args[1], layer);
                    trackingImage = ScaleGreen(layer, value);
                    cout << "Scaling the green channel of " << args[i] << " by " << value << " ...\n";
                } else {
                    Header result = ScaleGreen(trackingImage, value);
                    trackingImage = result;
                    cout << "... and scaling the green channel of previous step by " << value << " ...\n";
                }
            }
            i+=2;
        } else {return 1;}

        if (i+1 > args.size()) {
            break;
        }

    }
    // if (errorcount > 0) {
    //     return 1;
    // }
    Write(args[0], trackingImage);
    cout << "... " << "and saving output to " << args[0] << "!\n";
    return 0;
}




