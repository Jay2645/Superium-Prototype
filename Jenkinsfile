#!groovy

node( 'ue4 && vs2017' ) {
    agent any

	stage('Checkout') {
		checkout scm
    }

    stage('Generate Project Files') {
        bat "BuildScripts/Step1_StartBuild.bat"
    }

	stage('Build Project') {
		bat 'nuget restore Superium.sln'
		bat "\"${tool 'MSBuild'}\" Superium.sln /t:build /p:Platform=Win64;verbosity=diagnostic"
    }

    stage('Build Editor') {
        bat "BuildScripts/Step2_BuildFiles.bat"
    }

    stage('Build Lighting') {
        bat "BuildScripts/Step3_BuildLighting.bat"
    }

    stage('Cook and Deploy') {
        bat "BuildScripts/Step4_CookProject.bat"
    }
}