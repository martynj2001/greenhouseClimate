class MeasuresController < ApplicationController
  before_action :set_measure, only: [:show, :edit, :update, :destroy]
  
  # GET /measures
  # GET /measures.json
  def index

    require 'net/http'
    require 'json'

    @measures = Measure.all.order("created_at DESC")

    @weather_data = HTTParty.get('https://api.darksky.net/forecast/f3d5dcb788b60bc988305e4ba4bf0fe9/51.208168,-1.516628?exclude=hourly,minutely,alerts,flags&units=auto')

  end

  # Method call and processes the DarkSky API to produc local westher forcast
  # Selects teh correct icon to be displayed

  def weatherData 
    @weather_data = HTTParty.get('https://api.darksky.net/forecast/f3d5dcb788b60bc988305e4ba4bf0fe9/51.208168,-1.516628?exclude=hourly,minutely,alerts,flags&units=auto')
  end

  # GET /measures/1
  # GET /measures/1.json
  def show
  end

  # GET /measures/new
  def new
    @measure = Measure.new
  end

  # GET /measures/1/edit
  def edit
  end

  # POST /measures
  # POST /measures.json
  # Recieve data from Arduino and save in database
  def create
    @measure = Measure.new(measure_params)

    respond_to do |format|
      if @measure.save
        format.html { redirect_to @measure, notice: 'Measure was successfully created.' }
        format.json { render :show, status: :created, location: @measure }
        MeasureCleanupJob.set(wait: 36.hours).perform_later @measure
      else
        format.html { render :new }
        format.json { render json: @measure.errors, status: :unprocessable_entity }
      end
    end
  end

  # PATCH/PUT /measures/1
  # PATCH/PUT /measures/1.json
  def update
    respond_to do |format|
      if @measure.update(measure_params)
        format.html { redirect_to @measure, notice: 'Measure was successfully updated.' }
        format.json { render :show, status: :ok, location: @measure }
      else
        format.html { render :edit }
        format.json { render json: @measure.errors, status: :unprocessable_entity }
      end
    end
  end

  # DELETE /measures/1
  # DELETE /measures/1.json
  def destroy
    @measure.destroy
    respond_to do |format|
      format.html { redirect_to measures_url, notice: 'Measure was successfully destroyed.' }
      format.json { head :no_content }
    end
  end

  # Remove all table entries 
  # Use for development purposes.
  #GET /measures/remove_all
  def remove_all
    Measure.delete_all
    flash[:notice] = "You have removed all measurements!"
    redirect_to measures_path
  end

  private
    # Use callbacks to share common setup or constraints between actions.
    def set_measure
      @measure = Measure.find(params[:id])
    end

    # Never trust parameters from the scary internet, only allow the white list through.
    def measure_params
      params.require(:measure).permit(:temp_out, :temp_in, :humidity_out, :humidity_in, :soil_moisture)
    end
end
