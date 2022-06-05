#include "gdal.h"

int main()
{
    const char* pszDriverName = "ESRI Shapefile";
    GDALDriverH hDriver;
    GDALDatasetH hDS;
    OGRLayerH hLayer;
    OGRFieldDefnH hFieldDefn;
    double x, y;
    char szName[33];

    GDALAllRegister();

    hDriver = GDALGetDriverByName(pszDriverName);
    if (hDriver == NULL)
    {
        printf("%s driver not available.\n", pszDriverName);
        exit(1);
    }
    
    hDS = GDALCreate(hDriver, "point_out.shp", 0, 0, 0, GDT_Unknown, NULL);
    if (hDS == NULL)
    {
        printf("Creation of output file failed.\n");
        exit(1);
    }

    hLayer = GDALDatasetCreateLayer(hDS, "point_out", NULL, wkbPoint, NULL);
    if (hLayer == NULL)
    {
        printf("Layer creation failed.\n");
        exit(1);
    }

    hFieldDefn = OGR_Fld_Create("Name", OFTString);

    OGR_Fld_SetWidth(hFieldDefn, 32);

    if (OGR_L_CreateField(hLayer, hFieldDefn, TRUE) != OGRERR_NONE)
    {
        printf("Creating Name field failed.\n");
        exit(1);
    }

    OGR_Fld_Destroy(hFieldDefn);

    while (!feof(stdin)
        && fscanf(stdin, "%lf,%lf,%32s", &x, &y, szName) == 3)
    {
        OGRFeatureH hFeature;
        OGRGeometryH hPt;

        hFeature = OGR_F_Create(OGR_L_GetLayerDefn(hLayer));
        OGR_F_SetFieldString(hFeature, OGR_F_GetFieldIndex(hFeature, "Name"), szName);

        hPt = OGR_G_CreateGeometry(wkbPoint);
        OGR_G_SetPoint_2D(hPt, 0, x, y);

        OGR_F_SetGeometry(hFeature, hPt);
        OGR_G_DestroyGeometry(hPt);

        if (OGR_L_CreateFeature(hLayer, hFeature) != OGRERR_NONE)
        {
            printf("Failed to create feature in shapefile.\n");
            exit(1);
        }

        OGR_F_Destroy(hFeature);
    }

    GDALClose(hDS);

    return 0;
}