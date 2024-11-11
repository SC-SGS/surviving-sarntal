import graph;
import math;
import three;
import roundedpath;
//include "params.asy";
//import geometry;
// import settings;
// leftbutton=new string[] {"rotate","zoom","shift","pan"};
// middlebutton=new string[] {"menu"};
// rightbutton=new string[] {"zoom/menu","rotateX","rotateY","rotateZ"};
// wheelup=new string[] {"zoomin"};
// wheeldown=new string[] {"zoomout"};

settings.outformat = "pdf";

size(300);

void drawPlayer(pair pos, real scale) {
    // Define relative coordinates for the stick figure parts, scaled by `scale`
    pair headCenter = (0, 0.9 * scale);      // Head center point
    real headRadius = 0.1 * scale;           // Head radius
    pair bodyStart = (0, 0.8 * scale);       // Start of the body
    pair bodyEnd = (0, 0.4 * scale);         // End of the body
    pair leftArm = (-0.2 * scale, 0.6 * scale);  // Left hand
    pair rightArm = (0.2 * scale, 0.6 * scale);  // Right hand
    pair leftLeg = (-0.2 * scale, 0);        // Left foot
    pair rightLeg = (0.2 * scale, 0);        // Right foot

    // Draw the head as a circle (shifted by pos and scaled)
    draw(circle(pos + headCenter, headRadius));

    // Draw the body as a line (shifted by pos and scaled)
    draw((pos + bodyStart) -- (pos + bodyEnd));

    // Draw the arms as lines (shifted by pos and scaled)
    draw((pos + bodyStart) -- (pos + leftArm));
    draw((pos + bodyStart) -- (pos + rightArm));

    // Draw the legs as lines (shifted by pos and scaled)
    draw((pos + bodyEnd) -- (pos + leftLeg));
    draw((pos + bodyEnd) -- (pos + rightLeg));
}

pair normalize(pair vector){
    real length = length(vector);
    return (vector.x/length, vector.y/length);
}

real resolution = 0.02;

real[] pointsX = {};
real[] pointsY = {};

real[] derX = {};
real[] derY = {};

real[] tValues = {};
string[] tLabels = {};

real[] hermiteSplineLeftValueCoeffs = {2,-3,0,1};
real[] hermiteSplineRightValueCoeffs = {-2,3,0,0};
real[] hermiteSplineLeftDerCoeffs = {1,-2,1,0};
real[] hermiteSplineRightDerCoeffs = {1,-1,0,0};

void calculateTValues() {
    tValues.push(0);
    tLabels.push("$t_0$");
    real currentT = 0;
    for (int i = 1; i < pointsX.length; ++i) {
        currentT += sqrt((pointsX[i] - pointsX[i-1])**2 + (pointsY[i] - pointsY[i-1])**2);
        tValues.push(currentT);
        tLabels.push("$t_{" + string(i) + "}$");
    }
}

void calculateDers() {
    for (int i = 1; i < pointsX.length; ++i) {
        pair der = (pointsX[i] - pointsX[i-1], pointsY[i] - pointsY[i-1]);
        der = normalize(der);
        derX.push(der.x);
        derY.push(der.y);
    }
}

void initializeSplineValues(real[] xValues, real[] yValues) {
    pair firstDer = (xValues[1] - xValues[0], yValues[1] - yValues[0]);
    firstDer = normalize(firstDer);

    derX.push(firstDer.x);
    derY.push(firstDer.y);

    pointsX = xValues;
    pointsY = yValues;

    calculateTValues();
    calculateDers();
}

real evaluatePolynomial(real x, real[] coefficients) {
    real result = 0;
    for (real coeff : coefficients) {
        result *= x;
        result += coeff;
    }
    return result;
}

/**
* Evaluates the hermiteSpline given by the left value, right value, left derivative and right derivative in the hermiteCoefficients 
* as well as the lower and upper t-value for this spline.
*
* hermiteCoefficients: {leftValue, rightValue, leftDer, rightDer}
* tBounds: {lowT, highT}
*/
real evaluateSpline(real t, real[] hermiteCoefficients, real[] tBounds) {
    real width = tBounds[1] - tBounds[0];
    real relativeT = (t - tBounds[0]) / width;
    real result = 0;
    result += evaluatePolynomial(relativeT, hermiteSplineLeftValueCoeffs) * hermiteCoefficients[0];
    result += evaluatePolynomial(relativeT, hermiteSplineRightValueCoeffs) * hermiteCoefficients[1];
    result += evaluatePolynomial(relativeT, hermiteSplineLeftDerCoeffs) * width * hermiteCoefficients[2];
    result += evaluatePolynomial(relativeT, hermiteSplineRightDerCoeffs) * width * hermiteCoefficients[3];

    return result;
}

pair evaluate2DSpline(real t) {
    int index = 0;
    while(index < tValues.length && t >= tValues[index]) {
        ++index;
    }

    real[] tBounds = {tValues[index - 1], tValues[index]};
    // write(tValues[index - 1]);
    // write(tValues[index]);
    // write(t);
    // write();
    real[] hermiteCoefficientsX = {pointsX[index - 1], pointsX[index], derX[index - 1], derX[index]};
    real[] hermiteCoefficientsY = {pointsY[index - 1], pointsY[index], derY[index - 1], derY[index]};

    real xValue = evaluateSpline(t, hermiteCoefficientsX, tBounds);
    real yValue = evaluateSpline(t, hermiteCoefficientsY, tBounds);

    pair result = (xValue, yValue);
    return result;
}

void drawTerrain() {
    real currentT = 0;
    while(currentT + resolution <= tValues[tValues.length - 1]) {
        draw(evaluate2DSpline(currentT) -- evaluate2DSpline(currentT + resolution), green);
        currentT += resolution;
    }

    for(int i = 0; i < pointsX.length; ++i) {
        pair point = (pointsX[i], pointsY[i]);
        dot(point, red);
    }

    xaxis("$x$", NoTicks);
    yaxis("$y$", NoTicks, ymin=-1.2);
}

void drawTerrainWithMarkings() {
    real currentT = 0;
    while(currentT + resolution <= tValues[tValues.length - 1]) {
        draw(evaluate2DSpline(currentT) -- evaluate2DSpline(currentT + resolution), green);
        currentT += resolution;
    }

    // Basepoints
    for(int i = 0; i < tValues.length; ++i) {
        draw((pointsX[i], 0) -- (pointsX[i], pointsY[i]), dashed+magenta);
        draw((0, pointsY[i]) -- (pointsX[i], pointsY[i]), dashed+magenta);
        dot((pointsX[i], pointsY[i]), red);
    }

    xaxis("$x$", NoTicks);
    yaxis("$y$", NoTicks, ymin=-1.2);
}

void hermiteSplinepiece() {
    real[] valuesX = {0, 1};
    real[] valuesY = {0, 1};
    real[] derivatives = {0.25, 0.25};

    real[] hermiteCoefficients = {valuesY[0], valuesY[1], derivatives[0], derivatives[1]};

    for(real x = valuesX[0]; x < valuesX[1]; x += resolution) {
        real first = evaluateSpline(x, hermiteCoefficients, valuesX);
        real second = evaluateSpline(x + resolution, hermiteCoefficients, valuesX);
        pair firstPoint = (x, first);
        pair secondPoint = (x + resolution, second);
        draw(firstPoint -- secondPoint, green);
    }

    // Calculate and draw tangents at the start and end points
    pair startPoint = (valuesX[0], valuesY[0]);
    pair endPoint = (valuesX[1], valuesY[1]);

    // Tangent at the start point
    pair startTangent = (1, derivatives[0]);
    draw((startPoint - 0.2 * startTangent) -- (startPoint + 0.2 * startTangent), blue);
    label("$h'(t_l)$", startPoint + (0, 0.01), N, blue);

    // Tangent at the end point
    pair endTangent = (1, derivatives[1]);
    draw((endPoint - 0.2 * endTangent) -- (endPoint + 0.2 * endTangent), blue);
    label("$h'(t_r)$", endPoint + (0, 0.01), N, blue);

    // Mark and label the start and end points
    dot(startPoint, red);
    label("$(t_l, h(t_l))$", startPoint, SE, red);
    dot(endPoint, red);
    label("$(t_r, h(t_r))$", endPoint, SE, red);

    // Add a label on the spline at its midpoint
    real midpointX = (valuesX[0] + valuesX[1]) / 2;
    real midpointY = evaluateSpline(midpointX, hermiteCoefficients, valuesX);
    pair midpoint = (midpointX, midpointY);
    label("$h$", midpoint, NW, green); // Label positioned above the spline in green
}

void hermiteSpline(real[] functionValues, real[] functionDerivatives, string yLabel) {
    // Draw Spline
    for(real t = tValues[0]; t < tValues[tValues.length - 1]; t += resolution) {
        int index = 0;
        while(index < tValues.length && t >= tValues[index]) {
            ++index;
        }
        real[] tBounds = {tValues[index - 1], tValues[index]};
        real[] hermiteCoefficients = {functionValues[index - 1], functionValues[index], functionDerivatives[index - 1], functionDerivatives[index]};

        real first = evaluateSpline(t, hermiteCoefficients, tBounds);
        real second = evaluateSpline(t + resolution, hermiteCoefficients, tBounds);
        pair firstPoint = (t, first);
        pair secondPoint = (t + resolution, second);
        if(yLabel == "$x$") {
            draw(firstPoint -- secondPoint, blue);
        } else if(yLabel == "$y$") {
            draw(firstPoint -- secondPoint, orange);
        } else {
            draw(firstPoint -- secondPoint, green);
        }
        
    }

    // Draw Basepoints
    for(int i = 0; i < tValues.length; ++i) {
        if(yLabel == "$x$") {
            draw((tValues[i], 0) -- (tValues[i], functionValues[i]), dashed+blue);
            dot((tValues[i], functionValues[i]), blue);
        } else if(yLabel == "$y$") {
            draw((tValues[i], 0) -- (tValues[i], functionValues[i]), dashed+orange);
            dot((tValues[i], functionValues[i]), orange);
        } else {
            draw((tValues[i], 0) -- (tValues[i], functionValues[i]), dashed+red);
            dot((tValues[i], functionValues[i]), red);
        }
        
    }

    xaxis("", NoTicks);
    for(int i = 0; i < tValues.length; ++i) {
        xtick(Label(tLabels[i]), tValues[i]);
    }
    yaxis(yLabel, NoTicks);
}

void hermiteSplineInPlanes() {
    for(real t = tValues[0]; t < tValues[tValues.length - 1]; t += resolution) {
        int index = 0;
        while(index < tValues.length && t >= tValues[index]) {
            ++index;
        }
        real[] tBounds = {tValues[index - 1], tValues[index]};
        real[] hermiteCoefficientsX = {pointsX[index - 1], pointsX[index], derX[index - 1], derX[index]};
        real[] hermiteCoefficientsY = {pointsY[index - 1], pointsY[index], derY[index - 1], derY[index]};

        real firstX = evaluateSpline(t, hermiteCoefficientsX, tBounds);
        real secondX = evaluateSpline(t + resolution, hermiteCoefficientsX, tBounds);
        real firstY = evaluateSpline(t, hermiteCoefficientsY, tBounds);
        real secondY = evaluateSpline(t + resolution, hermiteCoefficientsY, tBounds);

        triple firstPointX = (firstX, 0, t);
        triple secondPointX = (secondX, 0, t + resolution);
        triple firstPointY = (0, firstY, t);
        triple secondPointY = (0, secondY, t + resolution);
        triple firstPointT = (firstX, firstY, 0);
        triple secondPointT = (secondX, secondY, 0);
        draw(firstPointX -- secondPointX, blue);
        draw(firstPointY -- secondPointY, orange);
        draw(firstPointT -- secondPointT, green);
    }

    // Draw Basepoints
    for(int i = 0; i < tValues.length; ++i) {
        //X
        draw((0, 0, tValues[i]) -- (pointsX[i], 0, tValues[i]), dashed+blue);
        draw((pointsX[i], 0, tValues[i]) -- (pointsX[i], pointsY[i], tValues[i]), dashed+magenta);
        dot((pointsX[i], 0, tValues[i]), blue);

        //Y
        draw((0, 0, tValues[i]) -- (0, pointsY[i], tValues[i]), dashed+orange);
        draw((0, pointsY[i], tValues[i]) -- (pointsX[i], pointsY[i], tValues[i]), dashed+magenta);
        dot((0, pointsY[i], tValues[i]), orange);

        //T
        draw((pointsX[i], pointsY[i], tValues[i]) -- (pointsX[i], pointsY[i], 0), dashed+red);
        dot((pointsX[i], pointsY[i], 0), red);

    }
}

void hermiteSpline3D() {
    currentprojection=perspective(7,3,5);
    //currentprojection=perspective(0,0,10);
    for(int i = 0; i < pointsX.length; ++i) {
        triple point = (pointsX[i], pointsY[i], tValues[i]);
        dot(point, magenta);
    }

    real currentT = 0;
    while(currentT + resolution <= tValues[tValues.length - 1]) {
        pair first = evaluate2DSpline(currentT);
        pair second = evaluate2DSpline(currentT + resolution);
        draw((first.x, first.y, currentT) -- (second.x, second.y, currentT + resolution), magenta);
        currentT += resolution;
    }

    //xaxis
    draw((0, 0, 0)--(22, 0, 0), Arrow3);
    label("$x$", (22, 0, 0), N);

    //yaxis
    draw((0, 0, 0)--(0, 9, 0), Arrow3);
    label("$y$", (0, 9, 0), N);

    //zaxis
    draw((0, 0, 0)--(0, 0, 34), Arrow3);  // z-axis (from origin to (0,0,5))
    label("$t$", (0, 0, 34), N); // Label the z-axis at the end
    hermiteSplineInPlanes();
}

struct TerrainPhase {
    pair deltaV;
    real r;
    int m;
}

// Helper function to initialize TerrainPhase
TerrainPhase createTerrainPhase(pair deltaV, real r, int m) {
    TerrainPhase tp;
    tp.deltaV = deltaV;
    tp.r = r;
    tp.m = m;
    return tp;
}

pair rotateByAngle(pair vector, real angle) {
    return (cos(angle) * vector.x - sin(angle) * vector.y, sin(angle) * vector.x + cos(angle) * vector.y);
}

real padding = 0.02;
//real cornerRadius = 0.2;
real boxSize = 7;

real determineCornerRadius(TerrainPhase ph) {
    // string text = "$" + string(ph.m) + "$";
    // path[] textBoundingBox = texpath(text);
    // 
    // // Calculate the radius: Use the bounding box's dimensions
    // pair minPoint = min(textBoundingBox);   // Bottom-left corner of the bounding box
    // pair maxPoint = max(textBoundingBox);   // Top-right corner of the bounding box
    return 0.5;
    //return 0.5 * length(maxPoint - minPoint);  // Half the diagonal length
}

void terrainPhase(pair pos, TerrainPhase ph) {
    //defaultpen(linewidth(2pt));
    defaultpen(fontsize(8pt));
    real cornerRadius = determineCornerRadius(ph);

    pair minVector = rotateByAngle(ph.deltaV, -ph.r * pi);
    pair maxVector = rotateByAngle(ph.deltaV, ph.r * pi);

    draw(roundedpath((pos + (-boxSize/2, -boxSize/2))--(pos + (-boxSize/2, boxSize/2))--(pos + (boxSize/2, boxSize/2))--(pos + (boxSize/2, -boxSize/2))--cycle, cornerRadius), black);
    draw(roundedpath((pos + (-boxSize/2 - padding, -boxSize/2 - padding))--(pos + (-boxSize/2 - padding, boxSize/2 + padding))--(pos + (boxSize/2 + padding, boxSize/2 + padding))--(pos + (boxSize/2 + padding, -boxSize/2 - padding))--cycle, cornerRadius), white);
    //draw(box(pos - (boxSize/2, boxSize/2), pos + (boxSize/2, boxSize/2)));
    pair circleCenter = pos + (boxSize/2 - cornerRadius, boxSize/2 - cornerRadius);
    draw(Circle(circleCenter, cornerRadius));
    label("$" + string(ph.m) + "$", circleCenter);

    draw(pos -- (pos + minVector), red, arrow=EndArrow);
    draw(pos -- (pos + maxVector), red, arrow=EndArrow);
    draw(pos -- (pos + ph.deltaV), magenta, arrow=EndArrow);
    dot(pos);

    //defaultpen(linewidth(1pt));
    //defaultpen(fontsize(12pt));
}

void addLabelsTerrainPhase(pair pos, TerrainPhase ph) {
    //defaultpen(linewidth(2pt));
    //defaultpen(fontsize(20pt));

    real cornerRadius = determineCornerRadius(ph);

    pair minVector = rotateByAngle(ph.deltaV, -ph.r * pi);
    pair maxVector = rotateByAngle(ph.deltaV, ph.r * pi);
    pair circleCenter = pos + (boxSize/2 - cornerRadius, boxSize/2 - cornerRadius);

    label("$\delta v$", pos + ph.deltaV - (0.1, 0.25), magenta);
    label("$m$", circleCenter - (0, cornerRadius + 0.2));
    draw(arc(pos, pos + 0.5*minVector, pos + 0.5*maxVector), red);
    label("$-r\pi$", pos + 0.15 * (ph.deltaV + minVector), red);
    label("$r\pi$", pos + 0.15 * (ph.deltaV + maxVector), red);

    //defaultpen(linewidth(1pt));
    //defaultpen(fontsize(12pt));
}

void terrainPhaseJustVectors(pair pos, TerrainPhase ph) {
    //defaultpen(linewidth(2pt));
    //defaultpen(fontsize(20pt));

    pair minVector = rotateByAngle(ph.deltaV, -ph.r * pi);
    pair maxVector = rotateByAngle(ph.deltaV, ph.r * pi);

    draw(pos -- (pos + minVector), red, arrow=EndArrow);
    draw(pos -- (pos + maxVector), red, arrow=EndArrow);
    draw(pos -- (pos + ph.deltaV), magenta, arrow=EndArrow);
    dot(pos);
}

void drawSplinePart(TerrainPhase[] phases, real[] choosenAngles, pair legendPos, pair firstPos, int pointsToDraw) {
    drawPlayer(firstPos + (1.5,0.3), 1.5);
    real[] subsetX = {firstPos.x};
    real[] subsetY = {firstPos.y};
    pair currentPos = firstPos;
    int terrainPhaseIndex = 0;
    TerrainPhase currentPhase = phases[terrainPhaseIndex];

    for(int i = 1; i < pointsToDraw; ++i) {
        currentPos = currentPos + rotateByAngle(currentPhase.deltaV, choosenAngles[i-1]);
        currentPhase.m -= 1;
        subsetX.push(currentPos.x);
        subsetY.push(currentPos.y);
        if(currentPhase.m == 0 && terrainPhaseIndex < phases.length - 1) {
            terrainPhaseIndex += 1;
            currentPhase = phases[terrainPhaseIndex];
        }
    }

    initializeSplineValues(subsetX, subsetY);
    drawTerrain();

    if(pointsToDraw <= choosenAngles.length) {
        pair nextPos = currentPos + rotateByAngle(currentPhase.deltaV, choosenAngles[pointsToDraw - 1]);
        terrainPhaseJustVectors(currentPos, currentPhase);
        draw(currentPos -- nextPos, green, arrow=EndArrow);
    }

    dot(currentPos);
    terrainPhase(legendPos, currentPhase);

}

// void drawSplinePart(int pointCount, TerrainPhase ph, real[] x, real[] y, pair legendPos) {
//     real[] subsetX = {};
//     real[] subsetY = {};
//     pair pos = (x[pointCount - 1], y[pointCount - 1]);
//     
//     for(int i = 0; i < pointCount; ++i) {
//         subsetX.push(x[i]);
//         subsetY.push(y[i]);
//     }
// 
//     initializeSplineValues(subsetX, subsetY);
//     drawTerrain();
// 
//     if(pointCount < x.length) {
//         pair nextPos = (x[pointCount], y[pointCount]);
//         pair nextPos = (x[pointCount], y[pointCount]);
//         draw(pos -- nextPos, green, arrow=EndArrow);
//         terrainPhaseJustVectors(pos, ph);
//     }
// 
//     terrainPhase(legendPos, ph);
// }

//OUTPUT:

//oldTerrain();
//newTerrain();
//hermiteSplinepiece();

//Terrain Representation
real[] x1 = {0, 3, 4.7, 7, 9, 10,  5.5, 7.5, 8, 9, 10, 20};
real[] y1 = {7, 7, 7,   5, 5, 4.2, 4,   3,   2, 1, 0,  0};

initializeSplineValues(x1, y1);
//drawTerrain();
drawTerrainWithMarkings();
//hermiteSpline(pointsX, derX, "$x$");
//hermiteSpline(pointsY, derY, "$y$");
//hermiteSpline3D();

//Terrain Generation

TerrainPhase phExample = createTerrainPhase((3, 0), 1/4, 4);
//terrainPhase((0,0), phExample);
//addLabelsTerrainPhase((0,0), phExample);

pair legendPos = (33, 15);
pair firstPos = (0, 7);

TerrainPhase ph1 = createTerrainPhase((3, 0), 1/4, 4);
TerrainPhase ph2NoRetracing = createTerrainPhase((-2, 0), 1/3, 2);
TerrainPhase ph2Retracing = createTerrainPhase((-2, 0), 1/3, 3);
TerrainPhase ph3NoRetracing = createTerrainPhase(rotateByAngle((3, 0), pi/3), 7/12, 5);
TerrainPhase ph3Retracing = createTerrainPhase(rotateByAngle((3, 0), pi/3), 7/12, 7);
TerrainPhase ph4 = createTerrainPhase(rotateByAngle((2, 0), -pi/6), 1/12, 5);
TerrainPhase ph5 = createTerrainPhase((2, 0), 1/2, 8);

TerrainPhase[] phasesNoRetracing = {};
phasesNoRetracing.push(ph1);
phasesNoRetracing.push(ph2NoRetracing);
phasesNoRetracing.push(ph3NoRetracing);
phasesNoRetracing.push(ph4);
phasesNoRetracing.push(ph5);

TerrainPhase[] phasesRetracing = {};
phasesRetracing.push(ph1);
phasesRetracing.push(ph2Retracing);
phasesRetracing.push(ph3Retracing);

real[] anglesPh1 = {0, pi/6, -pi/9, -(7*pi)/36};
real[] anglesPh2NoRetracing = {pi/12, (11*pi)/36};
real[] anglesPh2Retracing = {pi/12, (11*pi)/36, 0};
real[] anglesPh3NoRetracing = {-(4*pi)/9, -pi/4, pi/12, pi/4, -pi/12};
real[] anglesPh3Retracing = {0, 0};
real[] anglesPh4 = {pi/12, 0, -pi/12, pi/12, pi/12};
real[] anglesPh5 = {pi/12, -pi/3, (13*pi)/36, -pi/4, (2*pi)/9, -pi/4, (5*pi)/18, pi/12};

real[] anglesNoRetracing = {};
anglesNoRetracing.append(anglesPh1);
anglesNoRetracing.append(anglesPh2NoRetracing);
anglesNoRetracing.append(anglesPh3NoRetracing);
anglesNoRetracing.append(anglesPh4);
anglesNoRetracing.append(anglesPh5);

real[] anglesRetracing = {};
anglesRetracing.append(anglesPh1);
anglesRetracing.append(anglesPh2Retracing);
anglesRetracing.append(anglesPh3Retracing);

//drawSplinePart(phasesRetracing, anglesRetracing, legendPos, firstPos, points);
//drawSplinePart(phasesNoRetracing, anglesNoRetracing, legendPos, firstPos, points);