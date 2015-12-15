//
//  colorquant.cpp
//  
//
//  Created by Allison Wang on 12/15/15.
//
//

#include <stdio.h>

// Quantize the color of the image
FloatImage colorQuant(const FloatImage &im, float q, float omega, float lambda, bool clamp) {
    
    // image in LAB format already
    FloatImage lab(im);
    
    // empty image with only luminosity and one channel
    FloatImage lum(im.width(), im.height(), 1);
    for (int x=0; x<im.width(); x++) {
        for (int y=0; y<im.height(); y++) {
            lum(x,y,0) = lab(x,y,0); // luminosity = first channel of lab
        }
    }
    
    // get sobel gradient by calculating the gradient magnitude
    FloatImage sobel = gradientMagnitude(lum, clamp);
    
    
    // create bins
    vector<float> q_bins;
    
    // helper variables
    float min = lum.min(); // min luminosity value
    float range = lum.max() - lum.min(); // range of luminosity values
    
    // assign values to the bins based on how many bins you want (q)
    for (int i=0; i<q; i++) {
        float value = min + range*i/(q-1);
        q_bins.push_back(value);
        
    }
    
    // for every pixel
    for (int x=0; x<im.width(); x++) {
        for (int y=0; y<im.height(); y++) {
            
            // linear mappping of sharpness to omega-lambda range
            float sharpness = sobel(x,y,0);
            sharpness *= omega-lambda;
            sharpness += lambda;
            
            
            // find q nearest
            float qnearest = 150;
            for (int i=0; i<q; i++) {
                
                // if distance between the pixel's luminosity and the bin's value
                // is the smallest, assign it with the new value
                if (fabs(lum(x,y,0)-q_bins[i]) < fabs(lum(x,y,0)-qnearest)) {
                    qnearest = q_bins[i];
                    
                }
            }
            
            // assign output with new value
            // apply tangent function to smoothly interpolate between bins
            lab(x,y,0) = qnearest + q_bins[1]/2*tanh(sharpness*(lum(x,y,0)-qnearest));
        }
    }
    
    // convert back to rgb
    FloatImage output = lab2rgb(lab);
    
    // output
    return output;
}

